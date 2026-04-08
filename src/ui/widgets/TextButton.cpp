//
// Created by fran on 08/04/2026.
//

#include "TextButton.hpp"

TextButton::TextButton(const sf::Vector2f position, const std::string &text, const sf::Color textColor, const unsigned int characterSize)
    : m_isInteractable(true)
    , m_currentState(WidgetState::Base)
    , m_spacing(20, 20)
    , m_onClickCallback(nullptr)
{
    m_font = ResourceManager::getInstance().getFont("assets/fonts/ShadowsIntoLightTwo-Regular.ttf");
    m_text.setFillColor(textColor);
    m_text.setFont(m_font);
    m_text.setString(text);
    m_text.setCharacterSize(characterSize);
    m_text.setPosition(position);

    const sf::FloatRect textBounds = m_text.getGlobalBounds();
    m_background.setSize(textBounds.getSize() + m_spacing);
    m_background.setPosition(textBounds.left - m_spacing.x / 2, textBounds.top - m_spacing.y / 2);
    m_background.setFillColor(sf::Color::Transparent);
    m_background.setOutlineThickness(2);
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

bool TextButton::isInteractable() const
{
    return m_isInteractable;
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

void TextButton::onBase()
{
    m_background.setOutlineColor(m_baseColor);
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
