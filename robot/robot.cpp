#include "robot.hpp"

// std
#include <thread>
#include <iostream>

Robot::Robot(char *_ipRobot, Commands *_commands, Parameters *_parameters)
{
    ipRobot = _ipRobot;
    commandsPtr = new Commands(*_commands);
    parametersPtr = new Parameters(*_parameters);

    std::thread threadRobot;
    threadRobot.detach();
}

Robot::~Robot()
{
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
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void Robot::UpdateParameters(Parameters *_parameters)
{
    *parametersPtr = *_parameters;
}

void Robot::Process()
{
    CreateAPI();
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
