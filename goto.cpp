#include "BaseCyclic.pb.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <ostream>
#include <SessionManager.h>
#include <DeviceConfigClientRpc.h>
#include <BaseClientRpc.h>
#include <TransportClientUdp.h>
#include <TransportClientTcp.h>
#include <BaseCyclicClientRpc.h>
#include <BaseClientRpc.h>
#include <RouterClient.h>


namespace k_api = Kinova::Api;

#define PORT 10000

#define PORT_REAL_TIME 10001

#define OPERATION_ANGLE 0.05f //augmenter si sa lag (peut-etre passer en parametre)

struct servos_manager{
  int count;
  float *servos;
  int *servo_done;
};

k_api::Base::BaseClient *base;

k_api::BaseCyclic::BaseCyclicClient *basecyclic;

k_api::DeviceConfig::DeviceConfigClient *device_config;

k_api::SessionManager *session_manager;

k_api::SessionManager *RTsession_manager;

k_api::RouterClient *router;

k_api::RouterClient *RTrouter;

k_api::TransportClientTcp *transport;

k_api::TransportClientUdp *RTtransport;

void example_api_creation(char **argv) {


    // -----------------------------------------------------------
    // How to create an API with the SessionManager, DeviceConfigClient and BaseClient services
    auto error_callback = [](k_api::KError err){ cout << "_________ callback error _________" << err.toString(); };
    transport = new k_api::TransportClientTcp();
    RTtransport = new k_api::TransportClientUdp();
    router = new k_api::RouterClient(transport, error_callback);
    RTrouter = new k_api::RouterClient(RTtransport, error_callback);
    transport->connect(argv[1], PORT);
    RTtransport->connect(argv[1], PORT_REAL_TIME);
    // Set session data connection information
    auto create_session_info = k_api::Session::CreateSessionInfo();
    create_session_info.set_username(argv[2]);
    create_session_info.set_password(argv[3]);
    create_session_info.set_session_inactivity_timeout(60000);   // (milliseconds)
    create_session_info.set_connection_inactivity_timeout(2000); // (milliseconds)

    // Session manager service wrapper
    std::cout << "Creating session for communication" << std::endl;
    session_manager = new k_api::SessionManager(router);
    session_manager->CreateSession(create_session_info);
    RTsession_manager = new k_api::SessionManager(RTrouter);
    RTsession_manager->CreateSession(create_session_info);
    
    std::cout << "Session created" << std::endl;

    // Create DeviceConfigClient and BaseClient
    device_config = new k_api::DeviceConfig::DeviceConfigClient(router);
    basecyclic = new k_api::BaseCyclic::BaseCyclicClient(RTrouter);
    base = new k_api::Base::BaseClient(router);
}

void prepare() {
        
	auto servomode = k_api::Base::ServoingModeInformation();
	servomode.set_servoing_mode(k_api::Base::ServoingMode::LOW_LEVEL_SERVOING);
        base->SetServoingMode(servomode);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void goto_angles(float *angles, servos_manager *servos, float velocity) {

  timespec t_first;
  timespec t_last;
  int i = 0;
  int frequency = velocity/OPERATION_ANGLE; //velocity = degrees/sec operation_angle = degrees velocity/operation_angle = 1/s (Hz)
  int interval = 1000000000 / frequency; // convertir Hertz en intervalles de nanosecondes
  int operations[servos->count*2];//IMPORTANT! [1,500,-1,300] ou 1 represente un mouvement positif et 500 le nombre d'operations necessaires
  int operationmax=0;
  k_api::BaseCyclic::Feedback base_feedback;
  k_api::BaseCyclic::Command base_command;

  base_feedback = basecyclic->RefreshFeedback();


  while (i < servos->count) {
    servos->servos[i] = base_feedback.actuators(i).position();
    base_command.add_actuators()->set_position(base_feedback.actuators(i).position());
    i++;
  }
  
  
  for (int x = 0; x < servos->count; x++){// loptimisation de cette partie ne devrait pas etre necessaire car elle est faite une fois par mouvement. A voir si on fait >20 goto_angle() par seconde genre
    //on a 4 possibilites
    float diff = angles[x] - servos->servos[x];
    if (abs(diff) < OPERATION_ANGLE) {
      servos->servo_done[x]=1; // will not operate on difference smaller than operation angle
    }
    
    else if (diff > 0) {
      if (diff < 180) {//1
        operations[(x * 2) - 1] = 1; // mouvement positif
	operations[x*2]=diff/OPERATION_ANGLE;
      }
      else {//2
	operations[(x * 2) - 1] = -1; // mouvement negatif
	operations[x*2]=(360-diff)/OPERATION_ANGLE; //va falloir trouver un moyen d'underflow si l'API le fait pas (ils le font probablement)
      }
    }
    else {
      if (diff > -180) {//3
	operations[(x * 2) - 1] = -1; // mouvement negatif
	operations[x*2]=abs(diff)/OPERATION_ANGLE;
      }
      else {//4 deterministique, on est obliger de tomber dans une des 4 options donc un peut faire quelquechose dinsecure comme le if ci-dessous
        operations[(x * 2) - 1] = 1;//mouvement positif
	operations[x*2]=(360-abs(diff))/OPERATION_ANGLE;
      }
    }
    if (operations[x * 2] > operationmax) {
      operationmax=operations[x*2];
    }
  }

  i=0;
  while (i<operationmax){
    clock_gettime(CLOCK_MONOTONIC, &t_first);
    // DO THINGS UNDER
    for (int x = 0; x < servos->count; x++) {
      if (!servos->servo_done[x]) {
        servos->servos[x] += (OPERATION_ANGLE * operations[(x * 2) - 1]);
	if(i==operations[x*2]){
	  servos->servo_done[x]=1;
        }
	base_command.mutable_actuators(x)->set_position(servos->servos[x]);
      }
    }
    basecyclic->Refresh_async(base_command,0);
    // DO THINGS ABOVE
    clock_gettime(CLOCK_MONOTONIC, &t_last);
    while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < interval) {
      clock_gettime(CLOCK_MONOTONIC, &t_last);
    }
    i++;
  }
}

void destroy_API()
{
    // Close API session
    session_manager->CloseSession();

    // Deactivate the router and cleanly disconnect from the transport object
    router->SetActivationStatus(false);
    transport->disconnect();

    // Destroy the API
    delete base;
    delete device_config;
    delete session_manager;
    delete router;
    delete transport;
}



int main(int argc, char *argv[]) {
  if(argc != 6){printf("usage: goto [host] [username] [password] [velocite] [angle1,angle2,110.50(max deux decimales),ect... en pair de meme taille que le nombre de servos]\n");exit(1);}
  example_api_creation(argv);
  prepare();
  float velocity;// deg par seconde
  sscanf(argv[4], "%f", &velocity);
  servos_manager servos{};
  servos.count=base->GetActuatorCount(0).count();
  int angles_read = 1;
  int size_command = strlen(argv[5]);
  for (int x = 0; x < size_command;x++) {
    if (argv[5][x] == ',') {
      angles_read++;
    }
  }
  int angle_pairs = (int)floor(angles_read/servos.count);
  if ((int)floor(angles_read / servos.count) > 0) {
    float angles[angle_pairs][servos.count]; //discard tout ce qui est pas en pair de servos.count
    char temp[6];
    memset(&temp,0,6);
    int o = 0; //compteur pour l'array temporaire
    int y = 0; // compteur vers servos.count
    int z = 0; //compteur vers la commande (i.e angles[z][y] et x est le charactere)
    for (int x = 0; x < size_command; x++) {
      if(argv[5][x]!=',') { //tellement insecure et stupide
        temp[o] = argv[5][x];
	o++;
      }
      else {
        sscanf(temp, "%f", &angles[z][y]);
        if (angles[z][y] > 360 || angles[z][y] < 0) {
	  printf("invalid angle");
          exit(1);
        }
        o = 0;
	memset(&temp,0,5);
        if (y == servos.count - 1) {
          y = 0;
	  z++;
	}
        else {
	  y++; 
        }
      }
    }
    sscanf(temp, "%f", &angles[z][y]);//read le dernier

    float a[servos.count];
    int b[servos.count];

    servos.servos = a;
    servos.servo_done = b;
    
    for (int i = 0; i < servos.count; i++) {
      servos.servos[i] = 0;
      servos.servo_done[i] = 0;
    }
    servos_manager *_servo = &servos;
    for (int x=0; x < angle_pairs; x++) {
      float *_angles = angles[x];
      goto_angles(_angles,_servo,velocity);
    }
  }
  else{printf("invalid number of angles passed. please pass at least %d for this robot\n",servos.count);}
  
  destroy_API();
}
