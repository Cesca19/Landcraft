//
// Created by fran on 11/03/2026.
//

#include "WorldView.hpp"

WorldView::WorldView()
{
}

void WorldView::initCamera(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY)
{
    m_camera = std::make_unique<Camera>(tileSizeX, tileSizeY, heightScale, projectionAngleX, projectionAngleY);
}

void WorldView::initTileMap(const std::vector<std::vector<Tile>> &tiles)
{
    m_tileMap = std::make_unique<TileMap>();
    m_tileMap->init(tiles, *m_camera);
}

void WorldView::update(float deltaTime)
{
}

void WorldView::draw(sf::RenderWindow &window)
{
    window.draw(*m_tileMap);
}
