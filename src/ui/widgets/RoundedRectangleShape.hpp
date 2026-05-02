//
// Created by fran on 14/04/2026.
//

#ifndef LANDCRAFT_ROUNDEDRECTANGLESHAPE_HPP
#define LANDCRAFT_ROUNDEDRECTANGLESHAPE_HPP

#include <SFML/Graphics/Shape.hpp>
#include <cmath>

class RoundedRectangleShape : public sf::Shape {
public:
    RoundedRectangleShape(const sf::Vector2f& size = sf::Vector2f(0, 0), float radius = 0, unsigned int cornerPointCount = 10);
    void setSize(const sf::Vector2f& size);
    sf::Vector2f getSize() const;
    void setCornersRadius(float radius);
    float getCornersRadius() const;
    virtual std::size_t getPointCount() const override;
    virtual sf::Vector2f getPoint(std::size_t index) const override;

private:
    sf::Vector2f m_size;
    float m_radius;
    unsigned int m_cornerPointCount;
};

#endif //LANDCRAFT_ROUNDEDRECTANGLESHAPE_HPP
