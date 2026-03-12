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
    void init(const std::string& mapName, float tileSizeX, float tileSizeY, float heightScale, 
                float projectionAngleX, float projectionAngleY);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
private:
    WorldModel m_worldModel;
    WorldView m_worldView;
};


#endif //LANDCRAFT_WORLDCONTROLLER_HPP