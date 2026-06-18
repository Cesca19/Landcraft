//
// Created by fran on 17/06/2026.
//

#include "MenuActionButton.hpp"
#include <algorithm>

namespace {
    constexpr float ICON_SIZE = 34.f;
    constexpr float ICON_LEFT_PADDING = 22.f;
    constexpr float TEXT_LEFT_PADDING = 72.f;
}

MenuActionButton::MenuActionButton(const sf::Vector2f position, const sf::Vector2f size,
    const std::string &iconPath, const std::string &title, const std::string &subtitle,
    const unsigned int titleSize, const unsigned int subtitleSize)
    : m_isVisible(true)
    , m_isSelected(false)
    , m_isInteractable(true)
    , m_currentState(WidgetState::Base)
    , m_position(position)
    , m_size(size)
    , m_background(size, 14.f, 10)
    , m_baseOutlineColor(sf::Color(220, 210, 240))
    , m_hoverOutlineColor(sf::Color(180, 150, 230))
    , m_focusOutlineColor(sf::Color(160, 120, 220))
    , m_pressOutlineColor(sf::Color(130, 95, 185))
    , m_selectedOutlineColor(sf::Color(160, 120, 220))
    , m_baseBackgroundColor(sf::Color::White)
    , m_hoverBackgroundColor(sf::Color(248, 246, 252))
    , m_focusBackgroundColor(sf::Color(248, 246, 252))
    , m_pressBackgroundColor(sf::Color(235, 225, 250))
    , m_selectedBackgroundColor(sf::Color(248, 246, 252))
    , m_titleBaseColor(sf::Color(100, 80, 150))
    , m_titleHoverColor(sf::Color(120, 95, 180))
    , m_subtitleBaseColor(sf::Color(140, 130, 175))
    , m_subtitleHoverColor(sf::Color(120, 110, 165))
    , m_iconBaseColor(sf::Color(110, 95, 150))
    , m_iconHoverColor(sf::Color(140, 110, 200))
    , m_onClickCallback(nullptr)
{
    m_background.setPosition(position);
    m_background.setFillColor(m_baseBackgroundColor);
    m_background.setOutlineColor(m_baseOutlineColor);
    m_background.setOutlineThickness(-2.f);

    m_iconSprite.setTexture(ResourceManager::getInstance().getTexture(iconPath));
    const sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    m_iconSprite.setOrigin(iconLocal.width / 2.f, iconLocal.height / 2.f);

    m_title.setFont(ResourceManager::getInstance().getFont("assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_title.setString(title);
    m_title.setCharacterSize(titleSize);
    m_title.setStyle(sf::Text::Bold);
    m_title.setFillColor(m_titleBaseColor);

    m_subtitle.setFont(ResourceManager::getInstance().getFont("assets/fonts/ComicRelief-Regular.ttf"));
    m_subtitle.setString(subtitle);
    m_subtitle.setCharacterSize(subtitleSize);
    m_subtitle.setFillColor(m_subtitleBaseColor);

    refreshLayout();
}

void MenuActionButton::initOutlineStatesColors(const sf::Color &baseColor, const sf::Color &hoverColor,
    const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &selectedColor)
{
    m_baseOutlineColor = baseColor;
    m_hoverOutlineColor = hoverColor;
    m_focusOutlineColor = focusColor;
    m_pressOutlineColor = pressColor;
    m_selectedOutlineColor = selectedColor;
    onBase();
}

void MenuActionButton::initBackgroundStatesColor(const sf::Color &baseColor, const sf::Color &hoverColor,
    const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &selectedColor)
{
    m_baseBackgroundColor = baseColor;
    m_hoverBackgroundColor = hoverColor;
    m_focusBackgroundColor = focusColor;
    m_pressBackgroundColor = pressColor;
    m_selectedBackgroundColor = selectedColor;
    onBase();
}

void MenuActionButton::initTextColors(const sf::Color &titleBaseColor, const sf::Color &titleHoverColor,
    const sf::Color &subtitleBaseColor, const sf::Color &subtitleHoverColor)
{
    m_titleBaseColor = titleBaseColor;
    m_titleHoverColor = titleHoverColor;
    m_subtitleBaseColor = subtitleBaseColor;
    m_subtitleHoverColor = subtitleHoverColor;
    onBase();
}

void MenuActionButton::initIconColors(const sf::Color &baseColor, const sf::Color &hoverColor)
{
    m_iconBaseColor = baseColor;
    m_iconHoverColor = hoverColor;
    onBase();
}

void MenuActionButton::initOnClickCallback(std::function<void()> callback)
{
    m_onClickCallback = std::move(callback);
}

bool MenuActionButton::isInteractable() const
{
    return m_isInteractable;
}

bool MenuActionButton::isVisible() const
{
    return m_isVisible;
}

void MenuActionButton::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

bool MenuActionButton::isSelected() const
{
    return m_isSelected;
}

void MenuActionButton::setSelected(const bool isSelected)
{
    m_isSelected = isSelected;
    onBase();
}

sf::FloatRect MenuActionButton::getBounds() const
{
    return m_background.getGlobalBounds();
}

sf::Vector2f MenuActionButton::getCenter() const
{
    const sf::FloatRect bounds = m_background.getGlobalBounds();
    return {bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f};
}

void MenuActionButton::update(float deltaTime)
{
}

void MenuActionButton::draw(sf::RenderWindow &window) const
{
    window.draw(m_background);
    window.draw(m_iconSprite);
    window.draw(m_title);
    window.draw(m_subtitle);
}

void MenuActionButton::setState(const WidgetState state)
{
    if (m_currentState == state)
        return;
    switch (state) {
        case WidgetState::Base: onBase(); break;
        case WidgetState::Focused: onFocus(); break;
        case WidgetState::Hovered: onHover(); break;
        case WidgetState::Pressed: onPress(); break;
    }
    m_currentState = state;
}

void MenuActionButton::handleKeyBoardEvents(const sf::Event &event)
{
}

bool MenuActionButton::shouldFocusOnClick() const
{
    return false;
}

void MenuActionButton::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    refreshLayout();
}

void MenuActionButton::setSize(const sf::Vector2f &size)
{
    m_size = size;
    refreshLayout();
}

void MenuActionButton::refreshLayout()
{
    m_background.setPosition(m_position);
    m_background.setSize(m_size);

    const sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    const float iconScale = ICON_SIZE / std::max(iconLocal.width, iconLocal.height);
    m_iconSprite.setScale(iconScale, iconScale);
    m_iconSprite.setPosition(
        m_position.x + ICON_LEFT_PADDING + ICON_SIZE / 2.f,
        m_position.y + m_size.y / 2.f);

    m_title.setPosition(m_position.x + TEXT_LEFT_PADDING, m_position.y + 16.f);
    m_subtitle.setPosition(m_position.x + TEXT_LEFT_PADDING, m_position.y + 44.f);
}

void MenuActionButton::onBase()
{
    m_background.setOutlineColor(m_baseOutlineColor);
    m_background.setFillColor(m_isSelected ? m_selectedBackgroundColor : m_baseBackgroundColor);
    m_title.setFillColor(m_titleBaseColor);
    m_subtitle.setFillColor(m_subtitleBaseColor);
    m_iconSprite.setColor(m_iconBaseColor);
}

void MenuActionButton::onHover()
{
    m_background.setOutlineColor(m_hoverOutlineColor);
    m_background.setFillColor(m_hoverBackgroundColor);
    m_title.setFillColor(m_titleHoverColor);
    m_subtitle.setFillColor(m_subtitleHoverColor);
    m_iconSprite.setColor(m_iconHoverColor);
}

void MenuActionButton::onFocus()
{
    onHover();
    m_background.setOutlineColor(m_focusOutlineColor);
    m_background.setFillColor(m_focusBackgroundColor);
}

void MenuActionButton::onPress()
{
    m_background.setOutlineColor(m_pressOutlineColor);
    m_background.setFillColor(m_pressBackgroundColor);
    if (m_onClickCallback)
        m_onClickCallback();
}
