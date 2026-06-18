//
// Created by fran on 17/06/2026.
//

#include "Image.hpp"

Image::Image(const std::string &texturePath, const sf::Vector2f position,
    const sf::Vector2f displaySize)
    : m_isVisible(true)
    , m_position(position)
    , m_displaySize(displaySize)
    , m_useDisplaySize(displaySize.x > 0.f && displaySize.y > 0.f)
{
    setTexture(texturePath);
    setPosition(position);
}

void Image::setTexture(const std::string &texturePath)
{
    m_sprite.setTexture(ResourceManager::getInstance().getTexture(texturePath));
    m_sprite.setColor(sf::Color::White);
    if (m_useDisplaySize)
        applyDisplaySize();
    else
        m_sprite.setScale(1.f, 1.f);
}

void Image::setDisplaySize(const sf::Vector2f &displaySize)
{
    m_displaySize = displaySize;
    m_useDisplaySize = displaySize.x > 0.f && displaySize.y > 0.f;
    if (m_useDisplaySize)
        applyDisplaySize();
    else
        m_sprite.setScale(1.f, 1.f);
}

void Image::setScale(const sf::Vector2f &scale)
{
    m_useDisplaySize = false;
    m_sprite.setScale(scale);
}

void Image::setColor(const sf::Color &color)
{
    m_sprite.setColor(color);
}

void Image::setRotation(const float angleDegrees)
{
    m_sprite.setRotation(angleDegrees);
}

void Image::setOrigin(const sf::Vector2f &origin)
{
    m_sprite.setOrigin(origin);
}

bool Image::isInteractable() const
{
    return false;
}

bool Image::isVisible() const
{
    return m_isVisible;
}

void Image::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

bool Image::isSelected() const
{
    return false;
}

void Image::setSelected(bool isSelected)
{
}

sf::FloatRect Image::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

sf::Vector2f Image::getCenter() const
{
    const sf::FloatRect bounds = m_sprite.getGlobalBounds();
    return {bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f};
}

void Image::update(float deltaTime)
{
}

void Image::draw(sf::RenderWindow &window) const
{
    window.draw(m_sprite);
}

void Image::setState(const WidgetState state)
{
}

void Image::handleKeyBoardEvents(const sf::Event &event)
{
}

void Image::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    m_sprite.setPosition(position);
}

bool Image::shouldFocusOnClick() const
{
    return false;
}

void Image::applyDisplaySize()
{
    const sf::FloatRect localBounds = m_sprite.getLocalBounds();
    if (localBounds.width <= 0.f || localBounds.height <= 0.f)
        return;

    m_sprite.setScale(
        m_displaySize.x / localBounds.width,
        m_displaySize.y / localBounds.height);
}

void Image::onBase()
{
}

void Image::onHover()
{
}

void Image::onFocus()
{
}

void Image::onPress()
{
}
