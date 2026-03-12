//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDVIEW_HPP
#define LANDCRAFT_WORLDVIEW_HPP

#include <memory>
#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "TileMap.hpp"

class WorldView 
{
public:
    WorldView();
    void initCamera(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY);
    void initTileMap(const std::vector<std::vector<Tile>>& tiles);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
private:
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<TileMap> m_tileMap;
};


#endif //LANDCRAFT_WORLDVIEW_HPP