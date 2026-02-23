
//
// Created by fran on 03/02/2026.
//

#ifndef LANDCRAFT_WORLDVIEW_H
#define LANDCRAFT_WORLDVIEW_H

#include <SFML/Graphics.hpp>
#include "IsometricProjection.hpp"

class WorldView
{
public:
    WorldView(sf::Vector2f origin, sf::Vector2f size);
    ~WorldView();
    void init(sf::RenderWindow & window);
    void update(float deltaTime);
    void setSize(sf::Vector2f size);
    void resetCenter(sf::Vector2f origin);
    void zoom(int zoomDelta);
    void zoomAtMouse(float zoomDelta, sf::Vector2i mousePos);
    void startDragging(sf::Vector2i mousePos);
    void updateDragging(sf::Vector2i mousePos);
    void stopDragging();
    sf::Vector2f getCenter() const;
    sf::Vector2f getSize() const;
    // to do handle window resizing event;
    // might ave to create a base view class to do that
    // so the bacground view and this one could handle it automaticlly
    void moveTarget(const sf::Vector2f& offset);
    sf::Vector2f getTargetOrigin() const; // Pour lire la cible actuelle
private:
    void setCenter(const sf::Vector2f center);
    void updateWindowView();

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
    sf::View m_view;
    sf::RenderWindow *m_window;

    // make it global
    float m_zoomEpsilon = 0.001f;
    float m_movementEpsilon = 0.1f;
};


#endif //LANDCRAFT_WORLDVIEW_H
