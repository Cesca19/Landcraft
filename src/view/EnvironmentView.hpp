//
// Created by fran on 15/03/2026.
//

#ifndef LANDCRAFT_ENVIRONMENTVIEW_HPP
#define LANDCRAFT_ENVIRONMENTVIEW_HPP

#include <SFML/Graphics.hpp>
#include "Camera.hpp"

class EnvironmentView {
public:
    EnvironmentView();
    void init(sf::Vector2u windowSize);
    void update(const Camera &camera, sf::Vector2f viewCenter, sf::Vector2f viewSize,
        const sf::Vector2f& gizmoPosition, float gizmoSize, bool isViewMoving);
    void drawSkyBox(sf::RenderWindow& window) const;
    void drawWorldReference(sf::RenderWindow& window) const;
    void drawWorldGizmo(sf::RenderWindow& window) const;
private:
    void updateWorldReference(const Camera &camera, sf::Vector2f viewCenter, sf::Vector2f viewSize, bool isViewMoving);
    void updateWorldGizmo(const Camera &camera, const sf::Vector2f& gizmoPosition, float size, bool isViewMoving);
    // TO DO : update the skybox vertices pos on window size change
    void initSkyBox(sf::Vector2u windowSize);
    void initWorldGizmo();

    float m_maxScreenViewRadius;
    sf::VertexArray m_skyBox;
    sf::VertexArray m_worldReferenceVertexArray;
    // std::vector<sf::Vector2f> m_gizmoAxes;
    sf::VertexArray m_gizmoVertexArray;
};


#endif //LANDCRAFT_ENVIRONMENTVIEW_HPP