#include "utilities.h"

#include <string>
#include <vector>

class Robot
{
    public:
        Robot();
        ~Robot();

        extern std::string ipRobot;

    private:
        extern Commands* commandsPtr;
        extern Parameters* parametersPtr;

};