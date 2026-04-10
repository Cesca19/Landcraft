//
// Created by fran on 10/04/2026.
//

#include "Box.hpp"

Box::Box(const sf::Vector2f &position, const sf::Vector2f &size)
    : m_isVisible(true)
    , m_backgroundColor(sf::Color::Transparent)
    , m_outlineColor(sf::Color::Transparent)
{
    m_rectangle.setPosition(position);
    m_rectangle.setSize(size);
    m_rectangle.setFillColor(m_backgroundColor);
    m_rectangle.setOutlineColor(m_outlineColor);
    m_rectangle.setOutlineThickness(-2.f);
}

void Box::initColors(const sf::Color &backgroundColor, const sf::Color &outlineColor)
{
    m_backgroundColor = backgroundColor;
    m_outlineColor = outlineColor;
    m_rectangle.setFillColor(m_backgroundColor);
    m_rectangle.setOutlineColor(m_outlineColor);
}

bool Box::isInteractable() const
{
    return false;
}

bool Box::isVisible() const
{
    return m_isVisible;
}

void Box::setVisibility(bool isVisible)
{
    m_isVisible = isVisible;
}

bool Box::isSelected() const
{
    return false;
}

void Box::setSelected(bool isSelected)
{
    // This widget cannot be selected, so this function does nothing.
}

sf::FloatRect Box::getBounds() const
{
    return m_rectangle.getGlobalBounds();
}

sf::Vector2f Box::getCenter() const
{
    return m_rectangle.getPosition() + (m_rectangle.getSize() / 2.0f);
}

void Box::update(float deltaTime)
{
    // This widget does not have any dynamic behavior, so this function does nothing.
}

void Box::draw(sf::RenderWindow &window) const
{
    window.draw(m_rectangle);
}

void Box::setState(WidgetState state)
{
    // This widget does not have different states, so this function does nothing.
}

void Box::onBase()
{
}

void Box::onHover()
{
}

void Box::onFocus()
{
}

void Box::onPress()
{
}