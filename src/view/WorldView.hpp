//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDVIEW_HPP
#define LANDCRAFT_WORLDVIEW_HPP

#include <memory>
#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "TileMap.hpp"
#include "../utils/MathUtils.hpp"

class WorldView 
{
public:
    WorldView();
    void init(sf::Vector2f center, sf::Vector2f size);
    void initCamera(float tileSizeX, float tileSizeY, float heightScale, float projectionAngleX, float projectionAngleY);
    void initTileMap(const std::vector<std::vector<Tile>>& tiles);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void setSize(sf::Vector2f size);
    void setCenter(sf::Vector2f center);
    sf::Vector2f getCenter() const;
    sf::Vector2f getSize() const;
    void zoom(int zoomDelta);
    void zoomAtMouse(sf::RenderWindow& window, float zoomDelta);
    void startDragging(sf::RenderWindow& window);
    void updateDragging(sf::RenderWindow& window);
    void stopDragging();
    void moveTarget(const sf::Vector2f& offset);
    sf::Vector2f getTargetOrigin() const;
private:
    void updateViewCenter(const sf::Vector2f center);

    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<TileMap> m_tileMap;
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
};


#endif //LANDCRAFT_WORLDVIEW_HPP