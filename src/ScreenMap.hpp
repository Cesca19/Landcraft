#ifndef SCREEN_MAP_HPP
#define SCREEN_MAP_HPP
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <vector>
#include "TileCorner.hpp"

class ScreenMap
{
public:
    ScreenMap(int tileSizeX, int tileSizeY, int heightScale, sf::Vector2f translationOffset, int projectionAngleX, int projectionAngleY);
    ~ScreenMap();
    void update(sf::RenderWindow& window);
    void buildScreenMap(const std::vector<std::vector<TileCorner>> &worldMap);
    void draw(sf::RenderWindow& window);
private:
    void createVertexArrayMap();

    sf::Vector2f GetMouseWorldPosition(sf::Vector2i mouseScreenPosition);

    std::vector<ScreenTileCorner> getPointNeighbors(int x, int y);
    std::vector<ScreenTileCorner> getPointNeighborsInRadius(int x, int y, int radius);
    float distanceBetweenPoints(const sf::Vector2f& p1, const sf::Vector2f& p2);
    ScreenTileCorner getClosestNeighborInRadius(int worldX, int worldY, int radius, sf::Vector2f referencePoint);
    void getSelectedTilesCorner(sf::RenderWindow& window);

    void resetTilesCornerColors();
    void setSelectedTileCornersColors();

    float radToDeg(float rad);
    float degToRad(float deg);
    sf::Vector2f world_to_screen(int point3dX , int point3dY, int point3dZ);
    sf::Vector2f screen_to_world(int point2dX , int point2dY, int point2dZ);
   
    int m_projectionAngleX;
    int m_projectionAngleY;
    int m_tileSizeX;
    int m_tileSizeY;
    int m_heightScale;
    sf::Color m_selectedTilesColor = sf::Color::Magenta;
    sf::Color m_defaultTilesColor = sf::Color::Cyan;
    sf::Vector2f m_translationOffset;
    std::vector<std::vector<ScreenTileCorner>> m_map;
    sf::VertexArray m_vertexArrayMap;
    std::vector<ScreenTileCorner> m_selectedCorners;
};

#endif // SCREEN_MAP_HPP