//
// Created by fran on 14/04/2026.
//

#include "RoundedRectangleShape.hpp"

RoundedRectangleShape::RoundedRectangleShape(const sf::Vector2f &size, float radius, unsigned int cornerPointCount)
{
    m_size = size;
    m_radius = radius;
    m_cornerPointCount = cornerPointCount;
    update();
}

void RoundedRectangleShape::setSize(const sf::Vector2f &size)
{
    m_size = size; update();
}

sf::Vector2f RoundedRectangleShape::getSize() const
{
    return m_size;
}

void RoundedRectangleShape::setCornersRadius(float radius)
{
    m_radius = radius;
    update();
}

float RoundedRectangleShape::getCornersRadius() const
{
    return m_radius;
}

std::size_t RoundedRectangleShape::getPointCount() const
{
    return m_cornerPointCount * 4; // 4 coins
}

sf::Vector2f RoundedRectangleShape::getPoint(std::size_t index) const
{
    if (index >= m_cornerPointCount * 4) return {0, 0};

    const float deltaAngle = 90.0f / (m_cornerPointCount - 1);
    sf::Vector2f center;
    unsigned int centerIndex = index / m_cornerPointCount;
    static const float pi = 3.141592654f;

    switch (centerIndex) {
        case 0:
            center.x = m_size.x - m_radius;
            center.y = m_radius;
            break;
        case 1:
            center.x = m_radius;
            center.y = m_radius;
            break;
        case 2:
            center.x = m_radius;
            center.y = m_size.y - m_radius;
            break;
        case 3:
            center.x = m_size.x - m_radius;
            center.y = m_size.y - m_radius;
            break;
        default:
            break;
    }
    float angle = deltaAngle * (index % m_cornerPointCount) + (90.0f * centerIndex);
    float x = center.x + m_radius * std::cos(angle * pi / 180.0f);
    float y = center.y - m_radius * std::sin(angle * pi / 180.0f);
    return sf::Vector2f(x, y);
}
