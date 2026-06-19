//
// Created by fran on 17/06/2026.
//

#include "HelpSectionHeader.hpp"

HelpSectionHeader::HelpSectionHeader(const sf::Vector2f &position,
    const std::string &iconPath, const std::string &title,
    const unsigned int titleSize)
    : m_isVisible(true)
    , m_position(position)
    , m_iconColor(sf::Color(100, 80, 150))
{
    m_iconSprite.setTexture(ResourceManager::getInstance().getTexture(iconPath));
    const sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    m_iconSprite.setOrigin(iconLocal.width / 2.f, iconLocal.height / 2.f);

    m_title.setFont(ResourceManager::getInstance().getFont(
        "assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_title.setString(title);
    m_title.setCharacterSize(titleSize);
    m_title.setStyle(sf::Text::Bold);
    m_title.setFillColor(m_iconColor);

    refreshLayout();
}

void HelpSectionHeader::initIconColor(const sf::Color &color)
{
    m_iconColor = color;
    m_iconSprite.setColor(m_iconColor);
    m_title.setFillColor(m_iconColor);
}

void HelpSectionHeader::initTitleColor(const sf::Color &color)
{
    m_title.setFillColor(color);
}

bool HelpSectionHeader::isInteractable() const
{
    return false;
}

bool HelpSectionHeader::isVisible() const
{
    return m_isVisible;
}

void HelpSectionHeader::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

bool HelpSectionHeader::isSelected() const
{
    return false;
}

void HelpSectionHeader::setSelected(bool isSelected)
{
}

sf::FloatRect HelpSectionHeader::getBounds() const
{
    sf::FloatRect bounds = m_title.getGlobalBounds();
    const sf::FloatRect iconBounds = m_iconSprite.getGlobalBounds();
    bounds.left = std::min(bounds.left, iconBounds.left);
    bounds.width = std::max(bounds.width, iconBounds.width);
    bounds.height = std::max(bounds.height, iconBounds.height);
    return bounds;
}

sf::Vector2f HelpSectionHeader::getCenter() const
{
    const sf::FloatRect bounds = getBounds();
    return {bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f};
}

void HelpSectionHeader::update(float deltaTime)
{
}

void HelpSectionHeader::draw(sf::RenderWindow &window) const
{
    if (!m_isVisible)
        return;
    window.draw(m_iconSprite);
    window.draw(m_title);
}

void HelpSectionHeader::setState(const WidgetState state)
{
}

void HelpSectionHeader::handleKeyBoardEvents(const sf::Event &event)
{
}

bool HelpSectionHeader::shouldFocusOnClick() const
{
    return false;
}

void HelpSectionHeader::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    refreshLayout();
}

void HelpSectionHeader::refreshLayout()
{
    float iconSize = 35.f;
    float iconLeftPadding = 0.f;
    float iconTopPadding = 5.f;
    float textLeftPadding = 45.f;
    const sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    const float iconScale = iconSize / std::max(iconLocal.width, iconLocal.height);
    m_iconSprite.setScale(iconScale, iconScale);
    m_iconSprite.setColor(m_iconColor);
    m_iconSprite.setPosition(
        m_position.x + iconLeftPadding + iconSize / 2.f,
        m_position.y + iconTopPadding + iconSize / 2.f);
    m_title.setPosition(m_position.x + textLeftPadding, m_position.y + 2.f);
}

void HelpSectionHeader::onBase()
{
}

void HelpSectionHeader::onHover()
{
}

void HelpSectionHeader::onFocus()
{
}

void HelpSectionHeader::onPress()
{
}
