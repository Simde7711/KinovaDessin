#include "robot.h"

// json
#include <nlohmann/json.hpp>

// std
#include <iostream>
#include <fstream>
#include <string>

using json = nlohmann::json;

json ParsingJson(std::string filepath)
{
    std::ifstream ifstream(filepath);
    std::string fileContent((std::istreambuf_iterator<char>(ifstream)), std::istreambuf_iterator<char>());
    ifstream.close();

    json parsedJson = json::parse(fileContent);
    return parsedJson;
}

int main()
{
    std::string filePath;

    timespec t_first;
    timespec t_last;
                  
    std::vector<Robot> robots;
    while (1) 
    {
        clock_gettime(CLOCK_MONOTONIC, &t_first);
        
        // DO THINGS UNDER
        json jsonCommands = ParsingJson(filePath+"/commands.json");
        json jsonParameters = ParsingJson(filePath+"/parameters.json");

        for (int i = 0; 1 < jsonCommands["robots"].size(); i++)
        {
            bool enAction = false;
            for (const Robot &robot : robots) 
            {
                if (robot.ipRobot == jsonCommands["robots"][i]["ip"])
                {
                    enAction = true;
                }
            }

            if (!enAction)
            {
                Commands commands;

                size_t angle_count = jsonCommands["robots"][i]["commands"].size();
                float angles[angle_count];
                commands.angles = angles;

                for (size_t j = 0; j < angle_count; ++j) 
                {
                    commands.angles[j] = jsonCommands["robots"][i]["commands"][j].get<float>();
                }

                Parameters parameters;
                parameters.vitesse = 1;

                Robot robot
                (
                    &jsonCommands["robots"][i]["ip"].get<std::string>(), 
                    &commands,
                    &parameters
                );

                robots.push_back(robot);
            }
        }

        // DO THINGS ABOVE
        
        clock_gettime(CLOCK_MONOTONIC, &t_last);
        while ( ((( t_last.tv_sec - t_first.tv_sec ) *1000000000 ) + ( t_last.tv_nsec - t_first.tv_nsec ) ) < 1000000000) 
        {
            clock_gettime(CLOCK_MONOTONIC, &t_last);
        }
    
    }
}