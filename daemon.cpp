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
    while (true)
    {
        
    }
}