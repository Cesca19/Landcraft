//
// Created by fran on 08/04/2026.
//

#include "TextButton.hpp"

TextButton::TextButton(const sf::Vector2f position, const std::string &text, const unsigned int characterSize)
    : m_isVisible(true)
    , m_isSelected(false)
    , m_isInteractable(true)
    , m_currentState(WidgetState::Base)
    , m_baseBackgroundColor(sf::Color::White)
    , m_hoverBackgroundColor(sf::Color::White)
    , m_focusBackgroundColor(sf::Color::White)
    , m_pressBackgroundColor(sf::Color::White)
    , m_selectedBackgroundColor(sf::Color::White)
    , m_baseOutlineColor(sf::Color::Black)
    , m_hoverOutlineColor(sf::Color::Black)
    , m_focusOutlineColor(sf::Color::Black)
    , m_pressOutlineColor(sf::Color::Black)
    , m_baseTextColor(sf::Color::White)
    , m_hoverTextColor(sf::Color::White)
    , m_focusTextColor(sf::Color::White)
    , m_pressTextColor(sf::Color::White)
    , m_selectedTextColor(sf::Color::White)
    , m_padding(20, 20)
    , m_onClickCallback(nullptr)
{
    m_text.setFillColor(m_baseTextColor);
    m_text.setFont(ResourceManager::getInstance().getFont("assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_text.setString(text);
    m_text.setCharacterSize(characterSize);
    m_text.setPosition(position);

    sf::FloatRect textLocal = m_text.getLocalBounds();
    m_text.setOrigin(textLocal.left + textLocal.width / 2.0f, textLocal.top + textLocal.height / 2.0f);

    float bgWidth = textLocal.width + m_padding.x;
    float bgHeight = textLocal.height + m_padding.y;

    m_background.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_background.setPosition(position);
    m_background.setFillColor(m_baseBackgroundColor);
    m_background.setOutlineThickness(-2.f);

    float centerX = position.x + (bgWidth / 2.0f);
    float centerY = position.y + (bgHeight / 2.0f);
    m_text.setPosition(centerX, centerY);
}

void TextButton::initOutlineStatesColors(const sf::Color &baseColor, const sf::Color &hoverColor, 
    const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &selectedColor)
{
    m_baseOutlineColor = baseColor;
    m_hoverOutlineColor = hoverColor;
    m_focusOutlineColor = focusColor;
    m_pressOutlineColor = pressColor;
    m_selectedOutlineColor = selectedColor;

    m_background.setOutlineColor(m_baseOutlineColor);
}

void TextButton::initOnClickCallback(std::function<void()> callback)
{
    m_onClickCallback = std::move(callback);
}

void TextButton::initBackgroundStatesColor(const sf::Color &baseColor, const sf::Color &hoverColor, 
    const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &selectedColor)
{
    m_baseBackgroundColor = baseColor;
    m_hoverBackgroundColor = hoverColor;
    m_focusBackgroundColor = focusColor;
    m_pressBackgroundColor = pressColor;
    m_selectedBackgroundColor = selectedColor;

    m_background.setFillColor(m_baseBackgroundColor);
}

void TextButton::initTextColor(const sf::Color &baseTextColor, const sf::Color &hoverTextColor,
     const sf::Color &focusTextColor, const sf::Color &pressTextColor, const sf::Color &selectedTextColor)
{
    m_baseTextColor = baseTextColor;
    m_hoverTextColor = hoverTextColor;
    m_focusTextColor = focusTextColor;
    m_pressTextColor = pressTextColor;
    m_selectedTextColor = selectedTextColor;

    m_text.setFillColor(m_baseTextColor);
}

bool TextButton::isInteractable() const
{
    return m_isInteractable;
}

bool TextButton::isSelected() const
{
    return m_isSelected;
}

void TextButton::setSelected(const bool isSelected)
{
    m_isSelected = isSelected;
    if (m_isSelected) {
        m_background.setFillColor(m_selectedBackgroundColor);
        m_background.setOutlineColor(m_selectedOutlineColor);
        m_text.setFillColor(m_selectedTextColor);
    } else {
        m_background.setFillColor(m_baseBackgroundColor);
        m_background.setOutlineColor(m_baseOutlineColor);
        m_text.setFillColor(m_baseTextColor);
    }
}

sf::FloatRect TextButton::getBounds() const
{
    return m_background.getGlobalBounds();
}

sf::Vector2f TextButton::getCenter() const
{
    return m_background.getGlobalBounds().getPosition() + m_background.getGlobalBounds().getSize() / 2.0f;
}

void TextButton::update(float deltaTime)
{
}

void TextButton::draw(sf::RenderWindow &window) const
{
    window.draw(m_background);
    window.draw(m_text);
}

void TextButton::setState(const WidgetState state)
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

bool TextButton::isVisible() const
{
    return m_isVisible;
}

void TextButton::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
}

void TextButton::setPosition(const sf::Vector2f &position)
{
    m_text.setPosition(position);

    sf::FloatRect textLocal = m_text.getLocalBounds();
    m_text.setOrigin(textLocal.left + textLocal.width / 2.0f, textLocal.top + textLocal.height / 2.0f);

    float bgWidth = textLocal.width + m_padding.x;
    float bgHeight = textLocal.height + m_padding.y;

    m_background.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_background.setPosition(position);
    m_background.setFillColor(m_baseBackgroundColor);
    m_background.setOutlineThickness(-2.f);

    float centerX = position.x + (bgWidth / 2.0f);
    float centerY = position.y + (bgHeight / 2.0f);
    m_text.setPosition(centerX, centerY);
}

void TextButton::onBase()
{
    m_background.setOutlineColor(m_baseOutlineColor);
    m_background.setFillColor(m_baseBackgroundColor);
    m_text.setFillColor(m_baseTextColor);
    if (m_isSelected) {
        m_background.setFillColor(m_selectedBackgroundColor);
        m_text.setFillColor(m_selectedTextColor);
    }
}

void TextButton::onHover()
{
    m_background.setOutlineColor(m_hoverOutlineColor);
    m_background.setFillColor(m_hoverBackgroundColor);
    m_text.setFillColor(m_hoverTextColor);
}

void TextButton::onFocus()
{
    m_background.setOutlineColor(m_focusOutlineColor);
    m_background.setFillColor(m_focusBackgroundColor);
    m_text.setFillColor(m_focusTextColor);
}

void TextButton::onPress()
{
    m_background.setOutlineColor(m_pressOutlineColor);
    m_background.setFillColor(m_pressBackgroundColor);
    m_text.setFillColor(m_pressTextColor);
    if (m_onClickCallback)
        m_onClickCallback();
}
