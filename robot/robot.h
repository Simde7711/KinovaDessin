#include "utilities.h"

#include <string>
#include <vector>

class Robot
{
    public:
        Robot(std::string& _ipRobot, Commands& _commands, Parameters& _parameters);
        ~Robot();

        std::string ipRobot;

    private:
        Commands* commandsPtr;
        Parameters* parametersPtr;

};