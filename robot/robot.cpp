#include "robot.h"

// std
#include <thread>
#include <iostream>

Robot::Robot(std::string *_ipRobot, Commands *_commands, Parameters *_parameters)
{
    ipRobot = *_ipRobot;
    commandsPtr = new Commands(*_commands);
    parametersPtr = new Parameters(*_parameters);

    std::thread threadRobot(Process);
    threadRobot.detach();
}

Robot::~Robot()
{
    delete commandsPtr;
    delete parametersPtr;
}

void Robot::CreateAPI()
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
    create_session_info.set_username("DessinKinova");
    create_session_info.set_password("MOT_DE_PASSE");
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
	// std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void Robot::UpdateParameters(Parameters *_parameters)
{
    *parametersPtr = *_parameters;
}

void Robot::Process()
{
    CreateAPI();
    Prepare();

    // ici le while
}