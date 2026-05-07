//
// Created by fran on 08/04/2026.
//

#include "SpriteButton.hpp"

SpriteButton::SpriteButton(const std::string &iconPath, const sf::Vector2f position, const sf::Vector2f size, const std::string &highlightText,
    const int highlightTextSize)
    : m_spacing(5)
    , m_baseThickness(-3)
    , m_padding(20, 20)
    , m_highlightTextAlign(HighlightTextAlign::Top)
    , m_highlightTextColor(sf::Color::White)
    , m_backgroundBaseColor(sf::Color::Transparent)
    , m_iconBaseColor(sf::Color::White)
    , m_iconHoverColor(sf::Color::White)
    , m_iconFocusColor(sf::Color::White)
    , m_iconPressColor(sf::Color::White)
    , m_iconSelectedColor(sf::Color::White)
    , m_isVisible(true)
    , m_isSelected(false)
    , m_isInteractable(true)
    , m_isHighLightTextVisible(false)
    , m_didSupportContinuousClick(false)
    , m_elapsedTimeSinceClick(0)
    , m_continuousClickRepeatInterval(0.25)
    , m_currentState(WidgetState::Base)
    , m_onClickCallback(nullptr)
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
    m_highlightText.setFillColor(m_highlightTextColor);
    const sf::FloatRect textLocal = m_highlightText.getLocalBounds();
    m_highlightText.setOrigin(textLocal.left + textLocal.width / 2.0f, textLocal.top + textLocal.height);

    m_baseScale = sf::Vector2f(size.x / iconLocal.width, size.y / iconLocal.height);
    m_hoverScale = m_baseScale * 1.4f;
    m_pressScale = m_baseScale * 1.2f;
    m_baseScale = m_baseScale * 1.25f;

    m_background.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_background.setPosition(position);
    m_background.setFillColor(m_backgroundBaseColor);
    m_background.setOutlineThickness(m_baseThickness);

    initHighlightTextAlign(m_highlightTextAlign);
    onBase();
}

void SpriteButton::initHighLightTextColor(const sf::Color &highlightTextColor)
{
    m_highlightTextColor = highlightTextColor;
    m_highlightText.setFillColor(m_highlightTextColor);
}

void SpriteButton::initOutlineStatesColors(const sf::Color &baseColor, const sf::Color &hoverColor,
                                           const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color& selectedColor)
{
    m_baseColor = baseColor;
    m_hoverColor = hoverColor;
    m_focusColor = focusColor;
    m_pressColor = pressColor;
    m_selectedColor = selectedColor;
    onBase();
}

void SpriteButton::initBackgroundStatesColor(const sf::Color &baseColor, const sf::Color &hoverColor, 
    const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color& selectedColor)
{
    m_backgroundBaseColor = baseColor;
    m_backgroundHoverColor = hoverColor;
    m_backgroundFocusColor = focusColor;
    m_backgroundPressColor = pressColor;
    m_backgroundSelectedColor = selectedColor;

    m_background.setFillColor(m_backgroundBaseColor);
}

void SpriteButton::initIconStatesColor(const sf::Color &baseColor, const sf::Color &hoverColor,
    const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &selectedColor)
{
    m_iconBaseColor = baseColor;
    m_iconHoverColor = hoverColor;
    m_iconFocusColor = focusColor;
    m_iconPressColor = pressColor;
    m_iconSelectedColor = selectedColor;

    m_iconSprite.setColor(m_iconBaseColor);
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

void SpriteButton::setContinuousClick(const bool didSupportContinuousClick, const float repeatInterval)
{
    m_didSupportContinuousClick = didSupportContinuousClick;
    m_continuousClickRepeatInterval = repeatInterval;
}

bool SpriteButton::isVisible() const
{
    return m_isVisible;
}

void SpriteButton::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

bool SpriteButton::isSelected() const
{
    return m_isSelected;
}

void SpriteButton::setSelected(const bool isSelected)
{
    m_isSelected = isSelected;
    m_isHighLightTextVisible = isSelected;
    if (m_isSelected) {
        m_background.setOutlineColor(m_selectedColor);
        m_background.setFillColor(m_backgroundSelectedColor);
        m_background.setOutlineThickness(m_baseThickness * 1.5);
        m_iconSprite.setColor(m_iconSelectedColor);

    }
    else {
        m_background.setOutlineColor(m_baseColor);
        m_background.setFillColor(m_backgroundBaseColor);
        m_background.setOutlineThickness(m_baseThickness);
        m_iconSprite.setColor(m_iconBaseColor);
    }
}

sf::FloatRect SpriteButton::getBounds() const
{
    return m_background.getGlobalBounds();
}

sf::Vector2f SpriteButton::getCenter() const 
{
    return m_background.getPosition() + (m_background.getSize() / 2.0f);
}

void SpriteButton::update(const float deltaTime)
{
    if (!m_didSupportContinuousClick
        || m_currentState != WidgetState::Pressed)
        return;
    m_elapsedTimeSinceClick += deltaTime;
    if (m_elapsedTimeSinceClick >= m_continuousClickRepeatInterval) {
        m_elapsedTimeSinceClick = 0.0f;
        onPress();
    }
}

void SpriteButton::draw(sf::RenderWindow &window) const 
{
    window.draw(m_background);
    window.draw(m_iconSprite);
    if (m_isHighLightTextVisible)
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

void SpriteButton::resetIconScale()
{
    m_baseScale /= 1.25f;
    m_hoverScale = m_baseScale * 1.3f;
    m_pressScale = m_baseScale * 1.1f;
    m_iconSprite.setScale(m_baseScale);
}

void SpriteButton::setPosition(const sf::Vector2f &position)
{
    const float bgWidth = m_background.getSize().x;
    const float bgHeight = m_background.getSize().y;

    m_background.setPosition(position);

    const float centerX = position.x + (bgWidth / 2.0f);
    const float centerY = position.y + (bgHeight / 2.0f);
    m_iconSprite.setPosition(centerX, centerY);

    initHighlightTextAlign(m_highlightTextAlign);
}

void SpriteButton::onBase()
{
    m_background.setOutlineColor(m_baseColor);
    m_background.setFillColor(m_backgroundBaseColor);
    m_iconSprite.setScale(m_baseScale);
    m_iconSprite.setColor(m_iconBaseColor);
    // m_highlightText.setFillColor(m_highlightTextColor);
    m_isHighLightTextVisible = false;

    if (m_isSelected) {
        m_background.setOutlineColor(m_selectedColor);
        m_background.setOutlineThickness(m_baseThickness * 2);
        m_isHighLightTextVisible = true;
    }
}

void SpriteButton::onHover()
{
    m_background.setOutlineColor(m_hoverColor);
    m_background.setFillColor(m_backgroundHoverColor);
    m_iconSprite.setScale(m_hoverScale);
    m_iconSprite.setColor(m_iconHoverColor);
    m_isHighLightTextVisible = true;
}

void SpriteButton::onFocus()
{
    m_background.setOutlineColor(m_focusColor);
    m_background.setFillColor(m_backgroundFocusColor);
    m_iconSprite.setScale(m_hoverScale);
    m_iconSprite.setColor(m_iconFocusColor);
    m_isHighLightTextVisible = true;
}

void SpriteButton::onPress()
{
    m_background.setOutlineColor(m_pressColor);
    m_background.setFillColor(m_backgroundPressColor);
    m_iconSprite.setScale(m_pressScale);
    m_iconSprite.setColor(m_iconPressColor);
    m_highlightText.setFillColor(m_highlightTextColor);
    m_isHighLightTextVisible = true;

    if (m_onClickCallback)
        m_onClickCallback();
    if (!m_didSupportContinuousClick)
        return;
    m_elapsedTimeSinceClick = 0.0f;
}
