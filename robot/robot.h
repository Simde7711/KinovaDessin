#include "structJson.h"

#include <string>
#include <vector>

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

};