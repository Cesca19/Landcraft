#ifndef SCREEN_MAP_HPP
#define SCREEN_MAP_HPP
#define _USE_MATH_DEFINES

#include <memory>
#include <vector>

#include "Tile.hpp"
#include "TileCorner.hpp"
#include "WorldMap.hpp"

class ScreenMap {
public:
    ScreenMap(int tileSizeX, int tileSizeY, int heightScale, sf::Vector2f translationOffset, int projectionAngleX,
              int projectionAngleY);

    ~ScreenMap();

    void update(float deltaTime, const sf::RenderWindow &window, SelectionMode selectionMode);

    void draw(sf::RenderWindow &window);

    void init(const std::string &mapFilepath);

    void setSelectedCornersHeight(int heightOffset);

    sf::Vector2f getScreenMapCenter() const;

    void rotateAroundZAxis(float angle);

    void resetZAxisRotation();

private:
    void rotateMapAroundZAxis(float angle);

    void rotateCornerAroundZAxis(float angle, ScreenTileCorner *corner);

    void initTilesCornersMap();

    void initTilesMap();

    void createTileFromTileCorner(int tileCornerX, int tileCornerY);

    void createVertexArrayMap();

    /**
     * Calculates the exact Tile Index under the mouse cursor, accounting for Map Rotation.
     *
     * @param mouseScreenPosition The mouse position relative to the window/view (pixels).
     * @return The coordinates of the tile under the mouse (e.g., x=4.0, y=5.0).
     */
    sf::Vector2f getMouseWorldPosition(sf::Vector2f mouseScreenPosition);

    void resetTilesCornerColors() const;

    void setSelectedTileCornersColors() const;

    std::vector<ScreenTileCorner *> getPointNeighbors(int x, int y) const;

    std::vector<ScreenTileCorner *> getPointNeighborsInRadius(int x, int y, int radius) const;

    ScreenTileCorner *getClosestNeighborCornerInRadius(sf::Vector2i pointWorldPosition,
                                                       sf::Vector2f pointScreenPosition, int radius);

    std::vector<Tile *> getClosestTilesInRadius(int x, int y, int radius) const;

    Tile *getSelectedTileInRadius(sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius) const;

    void getSelectedTiles(sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);

    void getSelectedTilesCorners(sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);

    void getSelectedCorners(const sf::RenderWindow &window, SelectionMode selectionMode);

    float radToDeg(float rad);

    float degToRad(float deg);

    float distanceBetweenPoints(const sf::Vector2f &p1, const sf::Vector2f &p2);

    /**
     * Projects a 3D point from World Space (Tile Grid) to 2D Screen Space (Pixels).
     * Applies isometric projection, tile scaling, and height offset.
     *
     * @param point3dX The X index on the tile grid.
     * @param point3dY The Y index on the tile grid.
     * @param point3dZ The Z height/elevation of the tile.
     * @return The corresponding 2D coordinates on the screen (in pixels).
     */
    sf::Vector2f world_to_screen(float point3dX, float point3dY, float point3dZ);

    /**
     * Un-projects a 2D point from Screen Space (Pixels) back to 3D World Space (Tile Grid).
     * This is the inverse operation of world_to_screen.
     *
     * @param point2dX The X position on the screen (pixels).
     * @param point2dY The Y position on the screen (pixels).
     * @param point2dZ The assumed Z plane for the intersection (usually 0 for ground picking).
     * @return The unscaled world coordinates (tile indices).
     */
    sf::Vector2f screen_to_world(int point2dX, int point2dY, int point2dZ);

    sf::Vector2f getPointScreenPosition(sf::Vector2f worldPosition, int worldHeight);

    sf::Vector2f rotateAroundZAxis(float angle, sf::Vector2f point);

    float m_epsilon = 0.5f;
    int m_projectionAngleX;
    int m_projectionAngleY;
    int m_tileSizeX;
    int m_tileSizeY;
    int m_heightScale;

    float m_rotationSpeed;
    float m_currentRotationAngle;
    float m_targetRotationAngle;

    sf::Color m_selectedTilesColor = sf::Color::Magenta;
    sf::Color m_defaultTilesColor = sf::Color::Cyan;

    std::vector<std::vector<std::unique_ptr<ScreenTileCorner> > > m_map;
    std::vector<ScreenTileCorner *> m_selectedCorners;
    std::vector<std::vector<std::unique_ptr<Tile> > > m_tilesMap;
    sf::VertexArray m_vertexArrayMap;
    std::shared_ptr<WorldMap> m_worldMap;
};

#endif // SCREEN_MAP_HPP
