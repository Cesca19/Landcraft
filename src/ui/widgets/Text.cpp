//
// Created by fran on 10/04/2026.
//

#include "Text.hpp"

Text::Text(const sf::Vector2f position, const std::string &content, int characterSize)
    : m_isVisible(true)
{
    m_text.setString(content);
    m_text.setPosition(position);
    m_text.setCharacterSize(characterSize);
    m_text.setFont(ResourceManager::getInstance().getFont("assets/fonts/ComicRelief-Regular.ttf"));
}

void Text::init(sf::Color color, sf::Uint32 style)
{
    m_text.setFillColor(color);
    m_text.setStyle(style);
}

void Text::setContent(const std::string &content)
{
    m_text.setString(content);
}

bool Text::isInteractable() const
{
    return false;
}

bool Text::isVisible() const
{
    return m_isVisible;
}

void Text::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

bool Text::isSelected() const
{
    return false;
}

void Text::setSelected(bool isSelected)
{
}

sf::FloatRect Text::getBounds() const
{
    return m_text.getGlobalBounds();
}

sf::Vector2f Text::getCenter() const
{
    return m_text.getGlobalBounds().getPosition() + m_text.getGlobalBounds().getSize() / 2.0f;
}

void Text::update(float deltaTime)
{
}

void Text::draw(sf::RenderWindow &window) const
{
    window.draw(m_text);
}

void Text::setState(WidgetState state)
{
}

void Text::onBase()
{
}

void Text::onHover()
{
}

void Text::onFocus()
{
}

void Text::onPress()
{
}
