#ifndef SCREEN_MAP_HPP
#define SCREEN_MAP_HPP
#define _USE_MATH_DEFINES

#include <memory>
#include <vector>

#include "Tile.hpp"
#include "TileCorner.hpp"
#include "WorldMap.hpp"

class ScreenMap
{
public:
    ScreenMap(int tileSizeX, int tileSizeY, int heightScale, sf::Vector2f translationOffset, int projectionAngleX, int projectionAngleY);
    ~ScreenMap();
    void update(const sf::RenderWindow& window, SelectionMode selectionMode);
    void draw(sf::RenderWindow& window);
    void init(const std::string &mapFilepath);
    void setSelectedCornersHeight(int heightOffset);
    sf::Vector2f getCenter() const;
private:
    void initTilesCornersMap();
    void initTilesMap();
    void createTileFromTileCorner(int tileCornerX, int tileCornerY);

    void createVertexArrayMap();
    sf::Vector2f GetMouseWorldPosition(sf::Vector2f mouseScreenPosition);
    void resetTilesCornerColors() const;
    void setSelectedTileCornersColors() const;

    std::vector<ScreenTileCorner*> getPointNeighbors(int x, int y) const;
    std::vector<ScreenTileCorner*> getPointNeighborsInRadius(int x, int y, int radius) const;
    ScreenTileCorner *getClosestNeighborCornerInRadius(sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius);
    std::vector<Tile *> getClosestTilesInRadius(int x, int y, int radius) const;
    Tile* getSelectedTileInRadius(sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius) const;
    void getSelectedTiles(sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void getSelectedTilesCorners(sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void getSelectedCorners(const sf::RenderWindow& window, SelectionMode selectionMode);


    float radToDeg(float rad);
    float degToRad(float deg);
    float distanceBetweenPoints(const sf::Vector2f& p1, const sf::Vector2f& p2);
    sf::Vector2f world_to_screen(int point3dX , int point3dY, int point3dZ);
    sf::Vector2f screen_to_world(int point2dX , int point2dY, int point2dZ);
    sf::Vector2f getPointScreenPosition(sf::Vector2i worldPosition, int worldHeight);
   
    int m_projectionAngleX;
    int m_projectionAngleY;
    int m_tileSizeX;
    int m_tileSizeY;
    int m_heightScale;
    sf::Vector2f m_translationOffset;

    sf::Color m_selectedTilesColor = sf::Color::Magenta;
    sf::Color m_defaultTilesColor = sf::Color::Cyan;

    std::vector<std::vector<std::unique_ptr<ScreenTileCorner>>> m_map;
    std::vector<ScreenTileCorner *> m_selectedCorners;
    std::vector<std::vector<std::unique_ptr<Tile>>> m_tilesMap;
    sf::VertexArray m_vertexArrayMap;
    std::shared_ptr<WorldMap> m_worldMap;
};

#endif // SCREEN_MAP_HPP