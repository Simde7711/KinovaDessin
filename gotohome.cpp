#include <cstdint>
#include <cstdio>
#include <cstdlib>
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

int iteratetotarget(float *threshholdsbot, float *threshholdstop,float *servos, int count, float velocity, float *midpoints) {
  int i=0;
      for (int x; x < count; x++) {
	if( servos[x] > threshholdsbot[x] || servos[x] < threshholdstop[x]){continue;i++;}
        if (servos[x] < midpoints[x]) {
	  servos[x]-=velocity;
	} else {
	  servos[x]+=velocity;
	}
      }
      if (i == count - 1) {
        i = 0;
        return 1;
      }
      return 0;
}


void homeandback(int frequency, float velocity) { //frequency en hertz

  k_api::BaseCyclic::Feedback base_feedback;
  k_api::BaseCyclic::Command  base_command;
  
  base_feedback = basecyclic->RefreshFeedback();
  
  const int servoscount = base->GetActuatorCount().count();
  int i=0;

  float thresh = 0.5f;

  float servos[servoscount];
  
  float servosoriginthreshbot[servoscount];
  float servosoriginthreshtop[servoscount];
  float servosoriginmidpoint[servoscount];
  float homethreshbot[servoscount]; for(int x; x<servoscount; x++){homethreshbot[x]=359.5;}; //devrait etre une structure
  float homethreshtop[servoscount]; for(int x; x<servoscount; x++){homethreshtop[x]=0.5;};
  float homemidpoints[servoscount]; for(int x; x<servoscount; x++){homethreshbot[x]=180;};

  while (i < servoscount) {
    servos[i] = base_feedback.actuators(i).position();
    base_command.add_actuators()->set_position(base_feedback.actuators(i).position());
    i++;
  }
  i=0;
  while (i < servoscount)
  {
    if ((servos[i] + thresh) > 360) {
      servosoriginthreshtop[i] = (servos[i]+thresh-360);
    } else {
      servosoriginthreshtop[i]=servos[i]+thresh;
    }
    if ((servos[i] - thresh) < 0) {
      servosoriginthreshbot[i] = (servos[i]-thresh+360);
    } else {
      servosoriginthreshbot[i] = servos[i]-thresh;
    }
    if ((servosoriginthreshbot[i] + 180) > 360) {
      servosoriginmidpoint[i]= (servosoriginthreshbot[i] + 180 - 360);
    } else {
      servosoriginmidpoint[i]= (servosoriginthreshbot[i] + 180);
    }
    i++;
  }
  i = 0;
  timespec t_first;
  timespec t_last;
  int8_t stage=0;
  int interval = 1000000000 / frequency; // convertir Hertz en intervalles de nanosecondes
  while (1) {
    clock_gettime(CLOCK_MONOTONIC, &t_first);
    
    // DO THINGS UNDER
    if (stage == 0) { //linterieur des stages ou on se rend a un point devrait etre une fonction avec le target comme parametre
      if(iteratetotarget(homethreshbot, homethreshtop, servos, servoscount, velocity,homemidpoints)){stage++;}
    } else if (stage) {
      if(iteratetotarget(servosoriginthreshbot, servosoriginthreshtop, servos, servoscount, velocity, servosoriginmidpoint)){break;}
    }
    basecyclic->Refresh_async(base_command,0);
    // DO THINGS ABOVE
    
    clock_gettime(CLOCK_MONOTONIC, &t_last);
    while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < interval) {
      clock_gettime(CLOCK_MONOTONIC, &t_last);
    }
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



int main(int argc, char **argv) {
  if(argc != 6){printf("usage: vibrate [host] [username] [password] [frequency] [velocite]\n");exit(1);}
  example_api_creation(argv);
  prepare();
  int frequency;
  float velocity;// deg par iteration le calcul pour la vitesse est comme suit (Hz * velocity) = degrees/seconde i.e (200hz * 0.1f) = 20 degrees par seconde
  sscanf(argv[4], "%d", &frequency);
  sscanf(argv[5], "%f", &velocity);
  homeandback(frequency, velocity);
  destroy_API();
}
