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

        Robot(const char *_ipRobot, Commands *_commands, Parameters *_parameters) {	    
    ipRobot = _ipRobot;
    commandsPtr = new Commands(*_commands);
    parametersPtr = new Parameters(*_parameters);

    std::thread threadRobot;
    threadRobot.detach();
        }
        void CreateAPI(char **argv);
       
    private:
        Commands *commandsPtr;
        Parameters *parametersPtr;

        void Process();
  void UpdateParameters(Parameters *_parameters);
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

void Robot::Process()
{
    Prepare();

    k_api::BaseCyclic::Feedback base_feedback;
    k_api::BaseCyclic::Command  base_command;

    base_feedback = basecyclic->RefreshFeedback();

    int servoscount = base->GetActuatorCount().count();
    int i = 0;

    float servos[servoscount];

    while (i < servoscount) 
    {
        servos[i] = base_feedback.actuators(i).position();
        base_command.add_actuators()->set_position(base_feedback.actuators(i).position());
        i++;
    }

    
         
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

  if (argc != 1) {
    printf("usage: daemon [filepath]");
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

        for (int i = 0; 1 < jsonCommands["robots"].size(); i++)
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

                size_t angle_count = jsonCommands["robots"][i]["commands"].size();
                float angles[angle_count];
                commands.angles = angles;

                for (size_t j = 0; j < angle_count; ++j) 
                {
                    commands.angles[j] = jsonCommands["robots"][i]["commands"][j].get<float>();
                }

                Parameters parameters;
                parameters.vitesse = 1;

	        const char *_jsonCommands = jsonCommands["robots"][i]["ip"].get<std::string>().c_str();
                Robot robot(
		_jsonCommands,
		&commands,
		&parameters);

		robot.CreateAPI(argv);
		robots.push_back(robot);
            }
        }

        // DO THINGS ABOVE
        
        clock_gettime(CLOCK_MONOTONIC, &t_last);
        while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < 1000000000) //1 seconde
        {
            clock_gettime(CLOCK_MONOTONIC, &t_last);
        }
    
    }
}
