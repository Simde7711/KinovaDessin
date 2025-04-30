#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ostream>
#include <SessionManager.h>
#include <DeviceConfigClientRpc.h>
#include <BaseClientRpc.h>
#include <TransportClientUdp.h>
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

k_api::RouterClient *router;

k_api::TransportClientUdp *transport;

void example_api_creation(char **argv) {


    // -----------------------------------------------------------
    // How to create an API with the SessionManager, DeviceConfigClient and BaseClient services
    auto error_callback = [](k_api::KError err){ cout << "_________ callback error _________" << err.toString(); };
    transport = new k_api::TransportClientUdp();
    router = new k_api::RouterClient(transport, error_callback);
    printf(argv[1]);
    transport->connect(argv[1], PORT_REAL_TIME);

    // Set session data connection information
    auto create_session_info = k_api::Session::CreateSessionInfo();
    printf(argv[2]);
    create_session_info.set_username(argv[2]);
    printf(argv[3]);
    create_session_info.set_password(argv[3]);
    create_session_info.set_session_inactivity_timeout(60000);   // (milliseconds)
    create_session_info.set_connection_inactivity_timeout(2000); // (milliseconds)

    // Session manager service wrapper
    std::cout << "Creating session for communication" << std::endl;
    session_manager = new k_api::SessionManager(router);
    session_manager->CreateSession(create_session_info);
    std::cout << "Session created" << std::endl;

    // Create DeviceConfigClient and BaseClient
    device_config = new k_api::DeviceConfig::DeviceConfigClient(router);
    basecyclic = new k_api::BaseCyclic::BaseCyclicClient(router);
    base = new k_api::Base::BaseClient(router);

}

void prepare() {
        
	auto servomode = k_api::Base::ServoingModeInformation();
	servomode.set_servoing_mode(k_api::Base::ServoingMode::LOW_LEVEL_SERVOING);
        base->SetServoingMode(servomode);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void vibrate(int timer_max,int frequency,float velocity_vibrate) { //timer_max en ms, frequency en hertz et velocity en degres par ?temps?

  k_api::BaseCyclic::Feedback base_feedback;
  k_api::BaseCyclic::Command  base_command;

  base_feedback = basecyclic->RefreshFeedback();

  int servoscount = base->GetActuatorCount().count();
  int i=0;

  float servos[servoscount];
  
  while (i < servoscount) {
    servos[i] = base_feedback.actuators(i).position();
    base_command.add_actuators()->set_position(base_feedback.actuators(i).position());
    i++;
  }
  
  i=0;
  timespec t_first;
  timespec t_last;
  int max = timer_max *((float)frequency / (float)1000); // convertir kHz et un temps (en ms) en nombre d'iterations. C'est
                                     // plus optimiser comme sa que avec un autre timer.
  frequency = 1000000000 / frequency; // convertir Hertz en intervalles de nanosecondes
  while (i<max) {
    clock_gettime(CLOCK_MONOTONIC, &t_first);
    
    // DO THINGS UNDER
    for (int x = 0; x < servoscount; x++) {
      base_command.mutable_actuators(x)->set_velocity(velocity_vibrate);//la velocite EST EN QUOI! Selon l<utilisation de fmod() dans lexemple on peut deviner que cest en degres mais un velocite prend un facteur de temps!!! je devine par seconde.
    }
    velocity_vibrate=-velocity_vibrate;
    // DO THINGS ABOVE
    
    clock_gettime(CLOCK_MONOTONIC, &t_last);
    while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < frequency) {
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



int main(int argc, char **argv) {
  if(argc != 6){printf("usage: vibrate [host] [username] [password] [temps] [frequence]");exit(1);}
  example_api_creation(argv);
  prepare();
  int timer_max; sscanf(argv[4],"%d",&timer_max);
  int frequency; sscanf(argv[5], "%d",&frequency);
  vibrate(timer_max,frequency,0.1f);
  destroy_API();
}
