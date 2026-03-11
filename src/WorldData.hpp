//
// Created by fran on 03/03/2026.
//

#ifndef LANDCRAFT_WORLDDATA_HPP
#define LANDCRAFT_WORLDDATA_HPP
#include <string>
#include <vector>


class WorldData
{
public:
    WorldData();
    ~WorldData();
    void loadMap(std::string mapName);
private:
    std::vector<std::vector<int>> m_map;
};


#endif //LANDCRAFT_WORLDDATA_HPP