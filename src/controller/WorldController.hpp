//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDCONTROLLER_HPP
#define LANDCRAFT_WORLDCONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/WorldView.hpp"

class WorldController 
{
public:
    WorldController();
    void init(const std::string& mapName);
    void update(float deltaTime);
private:
    WorldModel m_worldModel;
    WorldView m_worldView;
};


#endif //LANDCRAFT_WORLDCONTROLLER_HPP