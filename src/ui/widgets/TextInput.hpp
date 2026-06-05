//
// Created by fran on 04/06/2026.
//

#ifndef LANDCRAFT_TEXTINPUT_HPP
#define LANDCRAFT_TEXTINPUT_HPP

#include <functional>
#include "IWidget.hpp"
#include "RoundedRectangleShape.hpp"

class TextInput : public IWidget {
public:
    TextInput(const sf::Vector2f &position, const sf::Vector2f &size, 
        const std::string &placeholder, int characterSize, bool isNumeric = false);
    bool isInteractable() const override;
    bool isVisible() const override;
    void setVisibility(bool isVisible) override;
    bool isSelected() const override;
    void setSelected(bool isSelected) override;

    void initBackgroundColors(const sf::Color& baseColor, const sf::Color& hoverColor, const sf::Color& focusColor, const sf::Color& pressColor);
    void initOutlineColors(const sf::Color& baseColor, const sf::Color& hoverColor, const sf::Color& focusColor, const sf::Color& pressColor);
    void initTextColors(const sf::Color& baseColor, const sf::Color& hover, const sf::Color& focusColor, const sf::Color& pressColor);
    void initCharacterLimit(unsigned int limit);

    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;
    void setPosition(const sf::Vector2f& position) override;

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void setState(WidgetState state) override;
    void handleKeyBoardEvents(const sf::Event& event) override;
    bool shouldFocusOnClick() const override;

    void initOnValidateCallback(std::function<void(const std::string&)> callback);
    void setNumericInput(bool isNumeric);
    std::string getText() const;
    void setText(const std::string& text);

private:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;
    void enableInput();

    std::string m_placeholder;
    bool m_isNumeric;
    bool m_isVisible;
    bool m_canType;
    unsigned int m_characterLimit;
    WidgetState m_currentState;
    std::function<void(const std::string&)> m_onValidateCallback;

    RoundedRectangleShape m_background;
    sf::Text m_text;
    sf::Color m_backgroundBaseColor;
    sf::Color m_backgroundHoverColor;
    sf::Color m_backgroundFocusColor;
    sf::Color m_backgroundPressColor;

    sf::Color m_outlineBaseColor;
    sf::Color m_outlineHoverColor;
    sf::Color m_outlineFocusColor;
    sf::Color m_outlinePressColor;

    sf::Color m_textBaseColor;
    sf::Color m_textHoverColor;
    sf::Color m_textFocusColor;
    sf::Color m_textPressColor;
};


#endif //LANDCRAFT_TEXTINPUT_HPP