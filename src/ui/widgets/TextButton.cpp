//
// Created by fran on 08/04/2026.
//

#include "TextButton.hpp"

TextButton::TextButton(const sf::Vector2f position, const std::string &text, const sf::Color textColor, const unsigned int characterSize)
    : m_isVisible(true)
    , m_isInteractable(true)
    , m_currentState(WidgetState::Base)
    , m_backgroundColor(sf::Color::Transparent)
    , m_padding(20, 20)
    , m_onClickCallback(nullptr)
{
    m_text.setFillColor(textColor);
    m_text.setFont(ResourceManager::getInstance().getFont("assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_text.setString(text);
    m_text.setCharacterSize(characterSize);
    m_text.setPosition(position);

    // const sf::FloatRect textBounds = m_text.getGlobalBounds();
    // m_background.setSize(textBounds.getSize() + m_padding);
    // m_background.setPosition(textBounds.left - m_padding.x / 2, textBounds.top - m_padding.y / 2);
    // m_background.setFillColor(sf::Color::Transparent);
    // m_background.setOutlineThickness(2);


    sf::FloatRect textLocal = m_text.getLocalBounds();
    m_text.setOrigin(textLocal.left + textLocal.width / 2.0f, textLocal.top + textLocal.height / 2.0f);

    float bgWidth = textLocal.width + m_padding.x;
    float bgHeight = textLocal.height + m_padding.y;

    m_background.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_background.setPosition(position);
    m_background.setFillColor(m_backgroundColor);
    m_background.setOutlineThickness(-2.f);

    float centerX = position.x + (bgWidth / 2.0f);
    float centerY = position.y + (bgHeight / 2.0f);
    m_text.setPosition(centerX, centerY);
}

void TextButton::initStatesColors(const sf::Color &baseColor, const sf::Color &hoverColor, const sf::Color &focusColor, const sf::Color &pressColor)
{
    m_baseColor = baseColor;
    m_hoverColor = hoverColor;
    m_focusColor = focusColor;
    m_pressColor = pressColor;

    m_background.setOutlineColor(m_baseColor);
}

void TextButton::initOnClickCallback(std::function<void()> callback)
{
    m_onClickCallback = std::move(callback);
}

void TextButton::initBackgroundColor(const sf::Color &color, const sf::Color &selectedColor)
{
    m_backgroundColor = color;
    m_selectedColor = selectedColor;
    m_background.setFillColor(m_backgroundColor);
}

bool TextButton::isInteractable() const
{
    return m_isInteractable;
}

bool TextButton::isSelected() const
{
    return m_isSelected;
}

void TextButton::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
    if (m_isSelected) {
        m_background.setFillColor(m_selectedColor);
    } else {
        m_background.setFillColor(m_backgroundColor);
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

void TextButton::onBase()
{
    m_background.setOutlineColor(m_baseColor);
    if (m_isSelected)
        m_background.setFillColor(m_selectedColor);
}

void TextButton::onHover()
{
    m_background.setOutlineColor(m_hoverColor);
}

void TextButton::onFocus()
{
    m_background.setOutlineColor(m_focusColor);
}

void TextButton::onPress()
{
    m_background.setOutlineColor(m_pressColor);
    if (m_onClickCallback)
        m_onClickCallback();
}
