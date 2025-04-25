#include "robot.h"
// #include JSON HEADER ET AUTRE LIBRAIRIES

std::string ipRobot = null;

Commands* commandsPtr = nullptr;
Parameters* parametersPtr = nullptr;

Robot::Robot(std::string _ipRobot, Commands _commands, Parameters _parameters)
{
    ipRobot = _ipRobot;

    commandsPtr = new Commands();
    *commandsPtr = _commands;
    
    parametersPtr; = new Parameters();
    *parametersPtr = _parameters;
}

Robot::~Robot()
{
    delete commandsPtr;
    delete parametersPtr;
}