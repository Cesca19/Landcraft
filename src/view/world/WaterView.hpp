//
// Created by fran on 19/05/2026.
//

#ifndef LANDCRAFT_WATERVIEW_HPP
#define LANDCRAFT_WATERVIEW_HPP

#include "Camera.hpp"
#include "../../utils/ResourceManager.hpp"

#include <stdexcept>
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

class WaterView {
public:
    WaterView();
    ~WaterView();
    void init(int nbCols, int nbRows, const sf::Vector2i& tilesSize, float waterHeight, const Camera &camera);
    void updatePositions(const Camera& camera);
    void update(float deltaTime);
    void draw(sf::RenderWindow &window) const;

    void setWaterHeight(float height);
    float getWaterHeight() const;
    void setIsVisible(bool visible);
    bool isVisible() const;

private:
    sf::VertexArray m_waterVertexArray;
    sf::Shader m_waterShader;
    sf::Texture m_waterTexture;
    sf::Color m_waterColor;
    float m_waterHeight;
    float m_totalTime;
    bool m_isVisible;
    sf::Vector2i m_tileSize;
    int m_nbCols;
    int m_nbRows;
};


#endif //LANDCRAFT_WATERVIEW_HPP