//
// Created by fran on 11/03/2026.
//

#include "WorldController.hpp"

WorldController::WorldController()
{
}

void WorldController::init(const std::string &mapName, float tileSizeX, float tileSizeY, float heightScale, 
                float projectionAngleX, float projectionAngleY)
{
    m_worldModel.loadMap(mapName);
    m_worldView.initCamera(tileSizeX, tileSizeY, heightScale, projectionAngleX, projectionAngleY);
    m_worldView.initTileMap(m_worldModel.getTiles());
}

void WorldController::update(float deltaTime)
{
}

void WorldController::draw(sf::RenderWindow &window)
{
    m_worldView.draw(window);
}
