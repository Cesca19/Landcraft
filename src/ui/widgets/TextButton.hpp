//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_TEXTBUTTON_HPP
#define LANDCRAFT_TEXTBUTTON_HPP

#include <functional>
#include "IWidget.hpp"

class TextButton : public IWidget
{
public:
    TextButton(sf::Vector2f position, const std::string& text, unsigned int characterSize = 30);
    void initOutlineStatesColors(const sf::Color& baseColor, const sf::Color& hoverColor, 
        const sf::Color& focusColor, const sf::Color& pressColor, const sf::Color& selectedColor);
    void initOnClickCallback(std::function<void()> callback);
    void initBackgroundStatesColor(const sf::Color& baseColor, const sf::Color& hoverColor, const sf::Color& focusColor, 
        const sf::Color& pressColor, const sf::Color& selectedColor);
    void initTextColor(const sf::Color& baseTextColor, const sf::Color& hoverTextColor, 
        const sf::Color& focusTextColor, const sf::Color& pressTextColor, const sf::Color& selectedTextColor);
    bool isInteractable() const override;

    bool isSelected() const override;
    void setSelected(bool isSelected) override;

    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;
    sf::Vector2f getPosition() const;

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void setState(WidgetState state) override;
    void handleKeyBoardEvents(const sf::Event& event) override;
    bool shouldFocusOnClick() const override;

    bool isVisible() const override;
    void setVisibility(bool isVisible) override;
    
    void setPosition(const sf::Vector2f& position) override;

    void setContent(const std::string& text);
protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;

    bool m_isVisible;
    bool m_isSelected;
    bool m_isInteractable;
    WidgetState m_currentState;

    sf::Text m_text;
    sf::RectangleShape m_background;
    sf::Color m_baseOutlineColor;
    sf::Color m_hoverOutlineColor;
    sf::Color m_focusOutlineColor;
    sf::Color m_pressOutlineColor;
    sf::Color m_selectedOutlineColor;
    sf::Color m_baseBackgroundColor;
    sf::Color m_selectedBackgroundColor;
    sf::Color m_hoverBackgroundColor;
    sf::Color m_focusBackgroundColor;
    sf::Color m_pressBackgroundColor;
    sf::Color m_baseTextColor;
    sf::Color m_hoverTextColor;
    sf::Color m_focusTextColor;
    sf::Color m_pressTextColor;
    sf::Color m_selectedTextColor;
    sf::Vector2f m_padding;
    sf::Vector2f m_position;
    std::function<void()> m_onClickCallback;
};


#endif //LANDCRAFT_TEXTBUTTON_HPP