//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDVIEW_HPP
#define LANDCRAFT_WORLDVIEW_HPP

#include <memory>
#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "TileMap.hpp"
#include "EnvironmentView.hpp"

class WorldView 
{
public:
    WorldView();
    void init(sf::Vector2f center, sf::Vector2f size);
    void initCamera(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY, sf::Vector2f worldPivot);
    void initTileMap(const std::vector<std::vector<Tile>>& tiles);
    void initEnvironment(sf::Vector2u windowSize);
    void update(float deltaTime, const std::vector<std::vector<Tile>>& tiles, const sf::RenderWindow &window);
    void draw(sf::RenderWindow& window) const;
    void onWindowResized(sf::Vector2u windowSize);

    bool isMoving() const;
    bool isRotating() const;

    void setSize(sf::Vector2f size);
    void setCenter(sf::Vector2f center);
    sf::Vector2f getCenter() const;
    sf::Vector2f getSize() const;
    const Camera &getCamera() const;

    void zoom(int zoomDelta);
    void zoomAtMouse(const sf::RenderWindow& window, float zoomDelta);

    void startDragging(const sf::RenderWindow& window);
    void updateDragging(const sf::RenderWindow& window);
    void stopDragging();
    void moveTarget(const sf::Vector2f& offset);
    sf::Vector2f getTargetOrigin() const;

    void rotatePitch(float angle) const;
    void rotateYaw(float angle) const;

    void startContinuousRotation(const sf::RenderWindow &window) const;
    void stopContinuousRotation() const;
    void updateContinuousRotation(const sf::RenderWindow &window, const std::vector<std::vector<Tile>>& tiles) const;

    void updateTileCorners(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<TileCorner *> &selectedCorners) const;
    void paintTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<Tile *> & tilesToPaint, int textureId) const;
    void paintTile(const std::vector<std::vector<Tile>>& worldTiles, Tile *tileToPaint, int textureId) const;
private:
    void updateViewCenter(sf::Vector2f center);

    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<TileMap> m_tileMap;
    std::unique_ptr<EnvironmentView> m_environmentView;
    sf::View m_view;

    float m_minZoom;
    float m_maxZoom;
    float m_currentZoom;
    float m_targetZoom;
    float m_zoomOffset;
    float m_zoomSpeed;

    sf::Vector2f m_currentCenter;
    sf::Vector2f m_targetCenter;
    float m_movementSpeed;

    sf::Vector2f m_dragStartWorldPos;
    bool m_isDragging;

    sf::Vector2f m_baseSize;

    // make it global
    float m_zoomEpsilon = 0.001f;
    float m_movementEpsilon = 0.1f;

    bool m_isMoving;
};

#endif //LANDCRAFT_WORLDVIEW_HPP