
//
// Created by fran on 03/02/2026.
//

#ifndef LANDCRAFT_WORLDVIEW_H
#define LANDCRAFT_WORLDVIEW_H

#include <SFML/Graphics.hpp>

class WorldView
{
public:
    WorldView(sf::Vector2f origin, sf::Vector2f size);
    ~WorldView();
    void init(sf::RenderWindow & window);
    void update(float deltaTime);
    void setSize(sf::Vector2f size);
    void setOrigin(sf::Vector2f origin);
    void zoom(int zoomDelta);
private:
    void rotate(int delta);
    void updateWindowView();

    float m_minZoom;
    float m_maxZoom;
    float m_currentZoom;
    float m_targetZoom;
    float m_zoomOffset;
    float m_zoomSpeed;

    int m_rotationOffset;

    sf::Vector2f m_baseSize;
    sf::View m_view;
    sf::RenderWindow * m_window;

    // make it global
    float m_epsilon = 0.01f;
};


#endif //LANDCRAFT_WORLDVIEW_H
