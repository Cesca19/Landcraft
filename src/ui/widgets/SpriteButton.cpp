//
// Created by fran on 08/04/2026.
//

#include "SpriteButton.hpp"

SpriteButton::SpriteButton(const std::string &iconPath, const sf::Vector2f position, const sf::Vector2f size, const std::string &highlightText,
    const int highlightTextSize)
    : m_isInteractable(true)
    , m_currentState(WidgetState::Base)
    , m_padding(20, 20)
    , m_spacing(5)
    , m_onClickCallback(nullptr)
{
    m_iconSprite.setTexture(ResourceManager::getInstance().getTexture(iconPath));
    m_highlightText.setFont(ResourceManager::getInstance().getFont("assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));

    m_highlightText.setString(highlightText);
    m_highlightText.setCharacterSize(highlightTextSize);

    sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    sf::FloatRect textLocal = m_highlightText.getLocalBounds();

    m_iconSprite.setOrigin(iconLocal.width / 2.0f, iconLocal.height / 2.0f);
    m_highlightText.setOrigin(textLocal.left + textLocal.width / 2.0f, textLocal.top + textLocal.height);

    m_baseScale = sf::Vector2f(size.x / iconLocal.width, size.y / iconLocal.height);
    m_hoverScale = m_baseScale * 0.8f;
    m_pressScale = m_baseScale * 0.7f;

    float bgWidth = size.x + m_padding.x;
    float bgHeight = size.y + m_padding.y;

    m_background.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_background.setPosition(position);
    m_background.setFillColor(sf::Color::Transparent);
    m_background.setOutlineThickness(-2.f);

    float centerX = position.x + (bgWidth / 2.0f);
    float centerY = position.y + (bgHeight / 2.0f);

    m_iconSprite.setPosition(centerX, centerY);

    sf::FloatRect textBounds = m_highlightText.getLocalBounds();
    m_highlightText.setPosition(centerX, position.y + bgHeight + textBounds.height + m_spacing);
    m_highlightText.setStyle(sf::Text::Bold | sf::Text::Italic);

    m_background.setFillColor(sf::Color::Transparent);
    m_background.setOutlineThickness(2);
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

void SpriteButton::initOnClickCallback(std::function<void()> callback)
{
    m_onClickCallback = std::move(callback);
}

bool SpriteButton::isInteractable() const
{
    return m_isInteractable;
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
    m_iconSprite.setScale(m_baseScale);

    // or add a bool should draw
    sf::Color transparentText = m_highlightTextColor;
    transparentText.a = 0;
    m_highlightText.setFillColor(transparentText);
}

void SpriteButton::onHover()
{
    m_background.setOutlineColor(m_hoverColor);
    m_iconSprite.setScale(m_hoverScale);
    m_highlightText.setFillColor(m_highlightTextColor);
}

void SpriteButton::onFocus()
{
    m_background.setOutlineColor(m_focusColor);
}

void SpriteButton::onPress()
{
    m_background.setOutlineColor(m_pressColor);
    m_iconSprite.setScale(m_pressScale);
    m_highlightText.setFillColor(m_highlightTextColor);

    if (m_onClickCallback)
        m_onClickCallback();
}
