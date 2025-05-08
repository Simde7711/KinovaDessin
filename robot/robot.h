#include "structJson.h"

// kinovaAPI
#include <SessionManager.h>
#include <DeviceConfigClientRpc.h>
#include <BaseClientRpc.h>
#include <TransportClientUdp.h>
#include <TransportClientTcp.h>
#include <BaseCyclicClientRpc.h>
#include <BaseClientRpc.h>
#include <RouterClient.h>

// std
#include <string>
#include <vector>

namespace k_api = Kinova::Api;

class Robot
{
    public:
        std::string ipRobot;
        
        Robot(std::string *_ipRobot, Commands *_commands, Parameters *_parameters);
        ~Robot();

        void UpdateParameters(Parameters *_parameters);

    private:
        Commands *commandsPtr;
        Parameters *parametersPtr;

        void Process();

        // API kinova
        void CreateAPI();
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