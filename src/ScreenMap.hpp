#ifndef SCREEN_MAP_HPP
#define SCREEN_MAP_HPP
// #define _USE_MATH_DEFINES

#include <memory>
#include <vector>

#include "Tile.hpp"
#include "TileCorner.hpp"
#include "WorldMap.hpp"
#include "IsometricProjection.hpp"

class ScreenMap {
public:
    ScreenMap(int tileSizeX, int tileSizeY, int heightScale, int projectionAngleX,
              int projectionAngleY);

    ~ScreenMap();

    void update(float deltaTime, const sf::RenderWindow &window, SelectionMode selectionMode);

    void draw(sf::RenderWindow &window);

    void init(const std::string &mapFilepath);

    void setSelectedCornersHeight(int heightOffset);

    sf::Vector2f getScreenMapCenter() const;

    // yaw rotation
    void rotateAroundZAxis(float angle);
    // pitch rotation
    void rotateAroundXAxis(float angle);

private:
    // yaw rotation
    void rotateMapAroundZAxis(float angle);
    void rotateCornerAroundZAxis(float angle, ScreenTileCorner *corner) const;

    // pitch rotation
    void rotateMapAroundXAxis(float angle);
    void rotateCornerAroundXAxis(float angle, ScreenTileCorner *corner) const;

    void initTilesCornersMap();
    void initTilesMap();
    void createTileFromTileCorner(int tileCornerX, int tileCornerY);

    void buildVertexArrayMap();

    /**
     * Calculates the exact Tile Index under the mouse cursor, accounting for Map Rotation.
     *
     * @param mouseScreenPosition The mouse position relative to the window/view (pixels).
     * @return The coordinates of the tile under the mouse (e.g., x=4.0, y=5.0).
     */
    sf::Vector2f getMouseWorldPosition(sf::Vector2f mouseScreenPosition) const;

    void resetTilesCornerColors() const;
    void setSelectedTileCornersColors() const;

    std::vector<ScreenTileCorner *> getPointNeighbors(int x, int y) const;
    std::vector<ScreenTileCorner *> getPointNeighborsInRadius(int x, int y, int radius) const;
    ScreenTileCorner *getClosestNeighborCornerInRadius(sf::Vector2i pointWorldPosition,
                                                       sf::Vector2f pointScreenPosition, int radius) const;
    std::vector<Tile *> getClosestTilesInRadius(int x, int y, int radius) const;
    Tile *getSelectedTileInRadius(sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius) const;

    void getSelectedTiles(sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void getSelectedTilesCorners(sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void getSelectedCorners(const sf::RenderWindow &window, SelectionMode selectionMode);

    float m_epsilon = 0.5f;

    int m_tileSizeX;
    int m_tileSizeY;
    int m_heightScale;
    IsometricProjection m_isometricProjection;

    float m_yawRotationSpeed;
    float m_currentYawRotationAngle;
    float m_targetYawRotationAngle;

    float m_pitchRotationSpeed;
    float m_currentPitchRotationAngle;
    float m_targetPitchRotationAngle;

    bool m_doesNeedVertexUpdate;
    sf::Color m_selectedTilesColor = sf::Color::Magenta;
    sf::Color m_defaultTilesColor = sf::Color::White;

    std::vector<std::vector<std::unique_ptr<ScreenTileCorner> > > m_map;
    std::vector<ScreenTileCorner *> m_selectedCorners;
    std::vector<std::vector<std::unique_ptr<Tile> > > m_tilesMap;
    sf::VertexArray m_vertexArrayMap;
    std::shared_ptr<WorldMap> m_worldMap;
};

#endif // SCREEN_MAP_HPP
