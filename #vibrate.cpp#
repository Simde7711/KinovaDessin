
#include "./include/client/SessionManager.h"
#include "./include/client_stubs/DeviceConfigClientRpc.h"
#include "./include/client_stubs/BaseClientRpc.h"

#include "./include/client/RouterClient.h"
#include "./include/client/TransportClientUdp.h"

#include "utilities.h"

namespace k_api = Kinova::Api;

void example_api_creation(int argc, char **argv)
{
    auto parsed_args = ParseExampleArguments(argc, argv);

    // -----------------------------------------------------------
    // How to create an API with the SessionManager, DeviceConfigClient and BaseClient services
    auto error_callback = [](k_api::KError err){ cout << "_________ callback error _________" << err.toString(); };
    auto transport = new k_api::TransportClientTcp();
    auto router = new k_api::RouterClient(transport, error_callback);
    transport->connect(parsed_args.ip_address, PORT);

    // Set session data connection information
    auto create_session_info = k_api::Session::CreateSessionInfo();
    create_session_info.set_username(parsed_args.username);
    create_session_info.set_password(parsed_args.password);
    create_session_info.set_session_inactivity_timeout(60000);   // (milliseconds)
    create_session_info.set_connection_inactivity_timeout(2000); // (milliseconds)

    // Session manager service wrapper
    std::cout << "Creating session for communication" << std::endl;
    auto session_manager = new k_api::SessionManager(router);
    session_manager->CreateSession(create_session_info);
    std::cout << "Session created" << std::endl;

    // Create DeviceConfigClient and BaseClient
    auto device_config = new k_api::DeviceConfig::DeviceConfigClient(router);
    auto base = new k_api::Base::BaseClient(router);

    // -----------------------------------------------------------
    // Now you're ready to use the API
    // ...
    // -----------------------------------------------------------
    // After you're done, here's how to tear down the API
}
void prepare(k_api::Base:BaseClient* base , k_api::Base::Base::ServoingMode mode)
{
	auto servomode = k_api::Base::ServoingModeInformation();
	servomode.set_servoing_mode(mode);
	base->SetServoingMode(servomode);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));


}
bool angular_move(k_api::Base::BaseClient* base)
{
	auto movement = k_api::Base::Action();
	
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

int main(int argc, char **argv)
{
    example_api_creation(argc, argv);
    angular_move();
	destroy_API();
}
