#include "robot.h"
// #include JSON HEADER ET AUTRE LIBRAIRIES

Robot::Robot(std::string& _ipRobot, Commands& _commands, Parameters& _parameters)
{
    ipRobot = _ipRobot;
    commandsPtr = new Commands(_commands);
    parametersPtr = new Parameters(_parameters);
}

Robot::~Robot()
{
    delete commandsPtr;
    delete parametersPtr;
}