#include "robot.h"
#include <thread>
// #include JSON HEADER ET AUTRE LIBRAIRIES

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

void Robot::UpdateParameters(Parameters *_parameters)
{
    *parametersPtr = *_parameters;
}

void Robot::Process()
{

}