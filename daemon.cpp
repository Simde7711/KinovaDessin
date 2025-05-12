#include "robot/structJson.hpp"

// kinovaAPI
#include <SessionManager.h>
#include <DeviceConfigClientRpc.h>
#include <BaseClientRpc.h>
#include <TransportClientUdp.h>
#include <TransportClientTcp.h>
#include <BaseCyclicClientRpc.h>
#include <BaseClientRpc.h>
#include <RouterClient.h>

// json
#include <cstdlib>
#include <nlohmann/json.hpp>

// std
#include <thread>
#include <iostream>
#include <fstream>
#include <string>

namespace k_api = Kinova::Api;

class Robot{

   public:
  const char *ipRobot; //correct que sa soit const
        
        ~Robot() {
	      // Delete les deux pointers vers les structs.
    delete commandsPtr;
    delete parametersPtr;

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

  Robot(const char *_ipRobot, Commands *_commands, Parameters *_parameters, char **argv) {	    
    ipRobot = _ipRobot;
    commandsPtr = new Commands(*_commands);
    parametersPtr = new Parameters(*_parameters);

    // std::thread threadRobot(Process);
    // threadRobot.detach();
    Process(argv);
        }
        void CreateAPI(char **argv);
        void UpdateParameters(Parameters *_parameters);
  int done=0;
    private:
        Commands *commandsPtr;
        Parameters *parametersPtr;

        void Process(char **argv);
        // API kinova
        void Prepare();
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
        #define OPERATION_ANGLE 0.05f
};

void Robot::CreateAPI(char **argv)
{
    auto error_callback = [](k_api::KError err){ cout << "_________ callback error _________" << err.toString(); };
    transport = new k_api::TransportClientTcp();
    RTtransport = new k_api::TransportClientUdp();
    router = new k_api::RouterClient(transport, error_callback);
    RTrouter = new k_api::RouterClient(RTtransport, error_callback);
    transport->connect(ipRobot, PORT);
    RTtransport->connect(ipRobot, PORT_REAL_TIME);

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

void Robot::Prepare()
{
    auto servomode = k_api::Base::ServoingModeInformation();
	servomode.set_servoing_mode(k_api::Base::ServoingMode::LOW_LEVEL_SERVOING);
    base->SetServoingMode(servomode);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void Robot::UpdateParameters(Parameters *_parameters)
{
    *parametersPtr = *_parameters;
}

void Robot::Process(char **argv) {
  CreateAPI(argv);
    Prepare();

  timespec t_first;
  timespec t_last;
    
    k_api::BaseCyclic::Feedback base_feedback;
    k_api::BaseCyclic::Command base_command;
    
    int velocity = 10;

    int frequency = velocity/OPERATION_ANGLE;
    int interval = 1000000000 / frequency;

    
    base_feedback = basecyclic->RefreshFeedback();
    int servoscount = base->GetActuatorCount().count();
    int i = 0;
    int servos_done[servoscount];
    float servos[servoscount];

    while (i < servoscount) 
    {
        servos[i] = base_feedback.actuators(i).position();
        base_command.add_actuators()->set_position(base_feedback.actuators(i).position());
        i++;
    }

    int operations[servoscount];
    int operation_sens[servoscount];
    int operationmax=0;

    for (int x = 0; x < servoscount; x++){// loptimisation de cette partie ne devrait pas etre necessaire car elle est faite une fois par mouvement. A voir si on fait >20 goto_angle() par seconde genre
    //on a 4 possibilites
    float diff = commandsPtr->angles[x] - servos[x];
    if (abs(diff) < OPERATION_ANGLE) {
      servos_done[x]=1; // will not operate on difference smaller than operation angle
    }
    else {
      servos_done[x]=0;
      if (diff > 0) {
	if (diff < 180) {//1
	  operation_sens[x] = 1; // mouvement positif
	  operations[x]=diff/OPERATION_ANGLE;
	}
	else {//2
	  operation_sens[x] = -1; // mouvement negatif
	  operations[x]=(360-diff)/OPERATION_ANGLE; //va falloir trouver un moyen d'underflow si l'API le fait pas (ils le font probablement)
      }
    }
      else {
	if (diff > -180) {//3
	operation_sens[x] = -1; // mouvement negatif
	operations[x]=abs(diff)/OPERATION_ANGLE;
      }
	else {//4 deterministique, on est obliger de tomber dans une des 4 options donc un peut faire quelquechose dinsecure comme le if ci-dessous
        operation_sens[x] = 1;//mouvement positif
	operations[x]=(360-abs(diff))/OPERATION_ANGLE;
      }
    }
      
    if (operations[x] > operationmax) {
      operationmax=operations[x];
    }
   }
}

  i=0;
  while (i<operationmax){
    clock_gettime(CLOCK_MONOTONIC, &t_first);
    // DO THINGS UNDER
    for (int x = 0; x < servoscount; x++) {
      if (!servos_done[x]) {
        servos[x] += (OPERATION_ANGLE * operation_sens[x]);
	if(i==operations[x]){
	  servos_done[x]=1;
        }
	base_command.mutable_actuators(x)->set_position(servos[x]);
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

  this->done=1;
}

using json = nlohmann::json;

json ParsingJson(std::string filepath)
{
    std::ifstream ifstream(filepath);
    std::string fileContent((std::istreambuf_iterator<char>(ifstream)), std::istreambuf_iterator<char>());
    ifstream.close();

    json parsedJson = json::parse(fileContent);
    return parsedJson;
}

int main(int argc, char **argv) {
  printf("%i",argc);
  if (argc != 4) {
    printf("usage: daemon [filepath] [username] [passwd] \n");
    exit(-1);
  }
  
  std::string filePath=argv[1];

    timespec t_first;
    timespec t_last;
                  
    std::vector<Robot> robots;
    while (1) 
    {
        clock_gettime(CLOCK_MONOTONIC, &t_first);
        
        // DO THINGS UNDER
        json jsonCommands = ParsingJson(filePath+"/commands.json");
        json jsonParameters = ParsingJson(filePath+"/parameters.json");

        std::cout << "json : " << jsonCommands["robots"].size() << '\n';
	
        for (int i = 0; i < jsonCommands["robots"].size(); i++)
        {
            bool enAction = false;
            for (const Robot &robot : robots) 
            {
                if (robot.ipRobot == jsonCommands["robots"][i]["ip"])
                {
                    enAction = true;
                }
            }

            if (!enAction)
            {
                Commands commands;

                const size_t angle_count = jsonCommands["robots"][i]["infoDessin"]["commands"].size();
                float angles[angle_count];
                commands.angles = angles;

                for (size_t j = 0; j < angle_count; ++j) 
                {
                    commands.angles[j] = jsonCommands["robots"][i]["infoDessin"]["commands"][j].get<float>();
                }

                Parameters parameters;
                parameters.vitesse = 1;

	        const char *ip = jsonCommands["robots"][i]["ip"].get<std::string>().c_str();
                Robot robot(ip, &commands, &parameters, argv);
                printf("here"); 
                robots.push_back(robot);
            }
        }
        for (int i = 0; i < robots.size(); i++) {
          if (robots[i].done) {
	    robots.erase(robots.begin()+i);
          }
	  //!TODO ERASE DANS LE JSON POUR PAS LE RECREER TODO!
        }
        // DO THINGS ABOVE
        
        clock_gettime(CLOCK_MONOTONIC, &t_last);
        while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < 1000000000) //1 seconde
        {
            clock_gettime(CLOCK_MONOTONIC, &t_last);
        }
    
    }
}
