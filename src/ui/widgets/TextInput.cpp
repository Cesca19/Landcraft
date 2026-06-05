//
// Created by fran on 04/06/2026.
//

#include "TextInput.hpp"

TextInput::TextInput(const sf::Vector2f &position, const sf::Vector2f &size, 
    const std::string &placeholder, int characterSize, bool isNumeric)
    : m_placeholder(placeholder)
    , m_isNumeric(isNumeric)
    , m_isVisible(true)
    , m_currentState(WidgetState::Base)
    , m_onValidateCallback(nullptr)
    , m_backgroundBaseColor(sf::Color::White)
    , m_backgroundHoverColor(sf::Color::White)
    , m_backgroundFocusColor(sf::Color::White)
    , m_backgroundPressColor(sf::Color::White)
    , m_outlineBaseColor(sf::Color::Black)
    , m_outlineHoverColor(sf::Color::Black)
    , m_outlineFocusColor(sf::Color::Black)
    , m_outlinePressColor(sf::Color::Black)
    , m_textBaseColor(sf::Color::Black)
    , m_textHoverColor(sf::Color::Black)
    , m_textFocusColor(sf::Color::Black)
    , m_textPressColor(sf::Color::Black)
    , m_background(size, 5, 5)
    , m_canType(false)
    , m_characterLimit(256)
{
    m_background.setPosition(position);
    m_background.setFillColor(m_backgroundBaseColor);
    m_background.setOutlineColor(m_outlineBaseColor);
    m_background.setOutlineThickness(-2.0f);
    sf::Vector2f padding =sf::Vector2f(10, 12.5f);

    m_text.setFillColor(m_textBaseColor);
    m_text.setFont(ResourceManager::getInstance().getFont("assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_text.setString(m_placeholder);
    m_text.setCharacterSize(characterSize - 4);
    m_text.setStyle(sf::Text::Italic | sf::Text::Bold);
    m_text.setPosition(position + padding);
}

bool TextInput::isInteractable() const
{
    return true;
}

bool TextInput::isVisible() const
{
    return m_isVisible;
}

void TextInput::setVisibility(bool isVisible)
{
    m_isVisible = isVisible;
}

bool TextInput::isSelected() const
{
    return false;
}

void TextInput::setSelected(bool isSelected)
{
}

void TextInput::initBackgroundColors(const sf::Color &baseColor, const sf::Color &hoverColor, const sf::Color &focusColor, const sf::Color &pressColor)
{
    m_backgroundBaseColor = baseColor;
    m_backgroundHoverColor = hoverColor;
    m_backgroundFocusColor = focusColor;
    m_backgroundPressColor = pressColor;
    if (m_currentState == WidgetState::Base)
        m_background.setFillColor(m_backgroundBaseColor);
}

void TextInput::initOutlineColors(const sf::Color &baseColor, const sf::Color &hoverColor, const sf::Color &focusColor, const sf::Color &pressColor)
{
    m_outlineBaseColor = baseColor;
    m_outlineHoverColor = hoverColor;
    m_outlineFocusColor = focusColor;
    m_outlinePressColor = pressColor;
    if (m_currentState == WidgetState::Base)
        m_background.setOutlineColor(m_outlineBaseColor);
}

void TextInput::initTextColors(const sf::Color &baseColor, const sf::Color &hoverColor, const sf::Color &focusColor, const sf::Color &pressColor)
{
    m_textBaseColor = baseColor;
    m_textHoverColor = hoverColor;
    m_textFocusColor = focusColor;
    m_textPressColor = pressColor;
    if (m_currentState == WidgetState::Base)
        m_text.setFillColor(m_textBaseColor);
}

void TextInput::initCharacterLimit(unsigned int limit)
{
    m_characterLimit = limit;
}

sf::FloatRect TextInput::getBounds() const
{
    return m_background.getGlobalBounds();
}

sf::Vector2f TextInput::getCenter() const
{
    return m_background.getPosition() + (m_background.getSize() / 2.0f);
}

void TextInput::setPosition(const sf::Vector2f &position)
{
    m_background.setPosition(position);
    m_text.setPosition(position + sf::Vector2f(10, 12.5f));
}

void TextInput::update(float deltaTime)
{
}

void TextInput::draw(sf::RenderWindow &window) const
{
    if (!m_isVisible)
        return;
    window.draw(m_background);
    window.draw(m_text);
}

void TextInput::setState(WidgetState state)
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
        default:
            break;
    }
    m_currentState = state;
}

void TextInput::handleKeyBoardEvents(const sf::Event &event)
{
    if (event.type == sf::Event::TextEntered && m_canType) {
        if (event.text.unicode == '\b') { // Handle backspace
            std::string currentText = m_text.getString();
            if (!currentText.empty()) {
                currentText.pop_back();
                m_text.setString(currentText);
            }
        } else if (event.text.unicode == '\r' || event.text.unicode == '\n') { // Handle enter key
            if (m_onValidateCallback) {
                m_onValidateCallback(m_text.getString());
            }
        } else if (event.text.unicode < 128) { // Handle regular character input
            char enteredChar = static_cast<char>(event.text.unicode);
            if (!m_isNumeric || (std::isdigit(enteredChar))) {
                std::string currentText = m_text.getString();
                if (currentText.length() < m_characterLimit) {
                    currentText += enteredChar;
                    m_text.setString(currentText);
                }
            }
        }
    }
}

bool TextInput::shouldFocusOnClick() const
{
    return true;
}

void TextInput::initOnValidateCallback(std::function<void(const std::string &)> callback)
{
    m_onValidateCallback = std::move(callback);
}

void TextInput::setNumericInput(bool isNumeric)
{
    m_isNumeric = isNumeric;
}

std::string TextInput::getText() const
{
    return (m_text.getString() == m_placeholder) ? "" : m_text.getString();
}

void TextInput::setText(const std::string &text)
{
    m_text.setString(text);
}

void TextInput::onBase()
{
    m_background.setFillColor(m_backgroundBaseColor);
    m_background.setOutlineColor(m_outlineBaseColor);
    m_text.setFillColor(m_textBaseColor);
    m_canType = false;
}

void TextInput::onHover()
{
    m_background.setFillColor(m_backgroundHoverColor);
    m_background.setOutlineColor(m_outlineHoverColor);
    m_text.setFillColor(m_textHoverColor);
}

void TextInput::onFocus()
{
    m_background.setFillColor(m_backgroundFocusColor);
    m_background.setOutlineColor(m_outlineFocusColor);
    m_text.setFillColor(m_textFocusColor);
    enableInput();
}

void TextInput::onPress()
{
    m_background.setFillColor(m_backgroundPressColor);
    m_background.setOutlineColor(m_outlinePressColor);
    m_text.setFillColor(m_textPressColor);
    enableInput();
}

void TextInput::enableInput()
{
    m_canType = true;
    m_text.setString(m_text.getString() == m_placeholder ? "" : m_text.getString());
    m_text.setStyle(sf::Text::Bold);
    m_text.setCharacterSize(m_text.getString() == m_placeholder ? m_text.getCharacterSize() + 4 : m_text.getCharacterSize());
}
