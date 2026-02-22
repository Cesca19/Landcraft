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
    ScreenMap(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX,
              float projectionAngleY);
    ~ScreenMap();
    void update(float deltaTime, const sf::RenderWindow &window, SelectionMode selectionMode);
    void draw(sf::RenderWindow &window);
    void init(const std::string &mapFilepath);
    void setSelectedCornersHeight(float heightOffset);
    sf::Vector2f getWorldMapCenter() const;
    sf::Vector2f getScreenMapCenter() const;

    // yaw rotation
    void rotateAroundZAxis(float angle);
    // pitch rotation
    void rotateAroundXAxis(float angle);

    void drawGizmo(sf::RenderWindow& window, const sf::Vector2f& uiPosition, float size);
    void drawWorldReference(sf::RenderWindow& window, sf::Vector2f viewCenter, sf::Vector2f viewSize);
    /*
     * Sets the world pivot point in screen coordinates.
     * This is used to define a reference point for camera movement and rotation.
     * @param worldPivotScreenPosition The screen coordinates of the pivot point.
     */
    void setWorldPivot(sf::Vector2f worldPivotScreenPosition);
private:
    void updateMap();
    void updateCorner(ScreenTileCorner *corner) const;

    // yaw rotation
    void rotateMapAroundZAxis(float angle);
    void rotateCornerAroundZAxis(float angle, ScreenTileCorner *corner) const;

    // pitch rotation
    void rotateMapAroundXAxis(float angle);

    void initTilesCornersMap();
    void initTilesMap();
    void createTileFromTileCorner(int tileCornerX, int tileCornerY);

    void buildVertexArrayMap();

    sf::Vector2f getPointScreenCoordinates(sf::Vector2f pointWorld, float height) const;
    /**
     * Calculates the exact Tile Index of the given screen position, accounting for Map Rotation.
     *
     * @param pointScreenPosition The screen position relative to the window/view (pixels).
     * @param height The assumed height of the point in world space
     * @return The coordinates of the tile under the given screen position (e.g., x=4.0, y=5.0).
     * these coordinates are in the non rotated world space, so they can be used directly to access the map data.
     */
    sf::Vector2f getPointTileCoordinates(sf::Vector2f pointScreenPosition, float height = 0) const;

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

    float m_tileSizeX;
    float m_tileSizeY;
    float m_heightScale;
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

    std::vector<sf::Vector2f> m_gizmoAxes;
    sf::VertexArray m_gizmoVertexArray;
    std::vector<sf::Vector2f> m_worldReferenceAxesNormals;
    sf::VertexArray m_worldReferenceVertexArray;
    float m_lastPitchRotationAngle;
    sf::Vector2f m_lastViewSize;
};

#endif // SCREEN_MAP_HPP
