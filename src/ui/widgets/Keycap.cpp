//
// Created by fran on 17/06/2026.
//

#include "Keycap.hpp"
#include <algorithm>

Keycap::Keycap(const sf::Vector2f &position, const std::string &label,
    const unsigned int characterSize)
    : m_isVisible(true)
    , m_position(position)
    , m_backgroundColor(sf::Color::White)
    , m_outlineColor(sf::Color(200, 195, 210))
    , m_textColor(sf::Color(100, 80, 150))
    , m_background({s_minWidth, s_height}, s_cornerRadius, 8)
{
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineColor(m_outlineColor);
    m_background.setOutlineThickness(-1.5f);

    m_label.setFont(ResourceManager::getInstance().getFont(
        "assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_label.setString(label);
    m_label.setCharacterSize(characterSize);
    m_label.setStyle(sf::Text::Bold);
    m_label.setFillColor(m_textColor);

    refreshLayout();
}

void Keycap::initColors(const sf::Color &backgroundColor, const sf::Color &outlineColor,
    const sf::Color &textColor)
{
    m_backgroundColor = backgroundColor;
    m_outlineColor = outlineColor;
    m_textColor = textColor;
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineColor(m_outlineColor);
    m_label.setFillColor(m_textColor);
}

void Keycap::setLabel(const std::string &label)
{
    m_label.setString(label);
    refreshLayout();
}

bool Keycap::isInteractable() const
{
    return false;
}

bool Keycap::isVisible() const
{
    return m_isVisible;
}

void Keycap::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

bool Keycap::isSelected() const
{
    return false;
}

void Keycap::setSelected(bool isSelected)
{
}

sf::FloatRect Keycap::getBounds() const
{
    return m_background.getGlobalBounds();
}

sf::Vector2f Keycap::getCenter() const
{
    const sf::FloatRect bounds = m_background.getGlobalBounds();
    return {bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f};
}

void Keycap::update(float deltaTime)
{
}

void Keycap::draw(sf::RenderWindow &window) const
{
    if (!m_isVisible)
        return;
    window.draw(m_background);
    window.draw(m_label);
}

void Keycap::setState(const WidgetState state)
{
}

void Keycap::handleKeyBoardEvents(const sf::Event &event)
{
}

bool Keycap::shouldFocusOnClick() const
{
    return false;
}

void Keycap::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    refreshLayout();
}

void Keycap::refreshLayout()
{
    const sf::FloatRect labelBounds = m_label.getLocalBounds();
    const float width = std::max(
        s_minWidth, labelBounds.width + s_horizontalPadding * 2.f);
    m_background.setSize({width, s_height});
    m_background.setPosition(m_position);
    m_label.setPosition(
        m_position.x + width / 2.f - labelBounds.width / 2.f - labelBounds.left,
        m_position.y + s_height / 2.f - labelBounds.height / 2.f - labelBounds.top);
}

void Keycap::onBase()
{
}

void Keycap::onHover()
{
}

void Keycap::onFocus()
{
}

void Keycap::onPress()
{
}
