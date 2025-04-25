// faudrait que les structs soit pour le deamon et les classe robots
#include <vector>

struct Position
{
    int x;
    int y;
    int i;
    bool draw;
};

struct Commands
{
    int scale;
    std::vector<Position>;
};

struct Parameters
{
    int vitesse;
    bool pause;
    bool cancel;
};