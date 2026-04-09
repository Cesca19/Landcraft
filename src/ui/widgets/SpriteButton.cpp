//
// Created by fran on 08/04/2026.
//

#include "SpriteButton.hpp"

SpriteButton::SpriteButton(const std::string &iconPath, const sf::Vector2f position, const sf::Vector2f size, const std::string &highlightText,
    const int highlightTextSize)
    : m_spacing(5)
    , m_padding(20, 20)
    , m_highlightTextAlign(HighlightTextAlign::Top)
    , m_backgroundBaseColor(sf::Color::Transparent)
    , m_isInteractable(true)
    , m_currentState(WidgetState::Base)
    , m_onClickCallback(nullptr)
    , m_isVisible(true)
{
    const float bgWidth = size.x + m_padding.x;
    const float bgHeight = size.y + m_padding.y;

    m_iconSprite.setTexture(ResourceManager::getInstance().getTexture(iconPath));
    const sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    m_iconSprite.setOrigin(iconLocal.width / 2.0f, iconLocal.height / 2.0f);
    const float centerX = position.x + (bgWidth / 2.0f);
    const float centerY = position.y + (bgHeight / 2.0f);
    m_iconSprite.setPosition(centerX, centerY);

    m_highlightText.setFont(ResourceManager::getInstance().getFont("assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_highlightText.setString(highlightText);
    m_highlightText.setCharacterSize(highlightTextSize);
    m_highlightText.setStyle(sf::Text::Bold | sf::Text::Italic);
    const sf::FloatRect textLocal = m_highlightText.getLocalBounds();
    m_highlightText.setOrigin(textLocal.left + textLocal.width / 2.0f, textLocal.top + textLocal.height);

    m_baseScale = sf::Vector2f(size.x / iconLocal.width, size.y / iconLocal.height);
    m_hoverScale = m_baseScale * 1.30f;
    m_pressScale = m_baseScale * 1.15f;

    m_background.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_background.setPosition(position);
    m_background.setFillColor(m_backgroundBaseColor);
    m_background.setOutlineThickness(-2.f);

    initHighlightTextAlign(m_highlightTextAlign);
}

void SpriteButton::initStatesColors(const sf::Color &baseColor, const sf::Color &hoverColor,
    const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &highlightTextColor)
{
    m_baseColor = baseColor;
    m_hoverColor = hoverColor;
    m_focusColor = focusColor;
    m_pressColor = pressColor;
    m_highlightTextColor = highlightTextColor;

    onBase();
}

void SpriteButton::initBackgroundStatesColor(const sf::Color &baseColor, const sf::Color &hoverColor, const sf::Color &focusColor, const sf::Color &pressColor)
{
    m_backgroundBaseColor = baseColor;
    m_backgroundHoverColor = hoverColor;
    m_backgroundFocusColor = focusColor;
    m_backgroundPressColor = pressColor;

    m_background.setFillColor(m_backgroundBaseColor);
}

void SpriteButton::initOnClickCallback(std::function<void()> callback)
{
    m_onClickCallback = std::move(callback);
}

void SpriteButton::initHighlightTextAlign(const HighlightTextAlign align)
{
    m_highlightTextAlign = align;

    const sf::FloatRect textLocal = m_highlightText.getLocalBounds();
    const sf::FloatRect bgBounds = m_background.getGlobalBounds();
    const float centerX = bgBounds.left + (bgBounds.width / 2.0f);
    const float centerY = bgBounds.top + (bgBounds.height / 2.0f);

    sf::Vector2f newTextPos;

    switch (m_highlightTextAlign) {
        case HighlightTextAlign::Top:
            newTextPos.x = centerX;
            newTextPos.y = bgBounds.top - m_spacing;
            break;
            
        case HighlightTextAlign::Down:
            newTextPos.x = centerX;
            newTextPos.y = bgBounds.top + bgBounds.height + m_spacing + textLocal.height; 
            break;
            
        case HighlightTextAlign::Left:
            newTextPos.x = bgBounds.left - m_spacing - (textLocal.width / 2.0f);
            newTextPos.y = centerY + (textLocal.height / 2.0f);
            break;
            
        case HighlightTextAlign::Right:
            newTextPos.x = bgBounds.left + bgBounds.width + m_spacing + (textLocal.width / 2.0f);
            newTextPos.y = centerY + (textLocal.height / 2.0f);
            break;
    }

    m_highlightText.setPosition(newTextPos);
}

bool SpriteButton::isInteractable() const
{
    return m_isInteractable;
}

bool SpriteButton::isVisible() const
{
    return m_isVisible;
}

void SpriteButton::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

sf::FloatRect SpriteButton::getBounds() const
{
    return m_background.getGlobalBounds();
}

sf::Vector2f SpriteButton::getCenter() const 
{
    return m_background.getPosition() + (m_background.getSize() / 2.0f);
}

void SpriteButton::update(float deltaTime) 
{
}

void SpriteButton::draw(sf::RenderWindow &window) const 
{
    window.draw(m_background);
    window.draw(m_iconSprite);
    window.draw(m_highlightText);
}

void SpriteButton::setState(const WidgetState state)
{
    if (m_currentState == state)
        return;
    switch (state) {
        case WidgetState::Base:
            onBase();
            break;
        case WidgetState::Focused:
            onFocus();
            break;
        case WidgetState::Hovered:
            onHover();
            break;
        case WidgetState::Pressed:
            onPress();
            break;
    }
    m_currentState = state;
}

void SpriteButton::onBase()
{
    m_background.setOutlineColor(m_baseColor);
    m_background.setFillColor(m_backgroundBaseColor);
    m_iconSprite.setScale(m_baseScale);

    // or add a bool should draw
    sf::Color transparentText = m_highlightTextColor;
    transparentText.a = 0;
    m_highlightText.setFillColor(transparentText);
}

void SpriteButton::onHover()
{
    m_background.setOutlineColor(m_hoverColor);
    m_background.setFillColor(m_backgroundHoverColor);
    m_iconSprite.setScale(m_hoverScale);
    m_highlightText.setFillColor(m_highlightTextColor);
}

void SpriteButton::onFocus()
{
    m_background.setOutlineColor(m_focusColor);
    m_background.setFillColor(m_backgroundFocusColor);

    // repeat hover state
}

void SpriteButton::onPress()
{
    m_background.setOutlineColor(m_pressColor);
    m_background.setFillColor(m_backgroundPressColor);
    m_iconSprite.setScale(m_pressScale);
    m_highlightText.setFillColor(m_highlightTextColor);

    if (m_onClickCallback)
        m_onClickCallback();
}
