//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_TEXTBUTTON_HPP
#define LANDCRAFT_TEXTBUTTON_HPP

#include "IWidget.hpp"

class TextButton : public IWidget
{
public:
    TextButton(sf::Vector2f position, const std::string& text, sf::Color textColor, unsigned int characterSize = 30);
    void initStatesColors(const sf::Color& baseColor, const sf::Color& hoverColor, 
        const sf::Color& focusColor, const sf::Color& pressColor);

    bool isInteractable() const override;
    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;
    void draw(sf::RenderWindow& window) const override;
    void setState(WidgetState state) override;

protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;

    bool m_isInteractable;
    WidgetState m_currentState;

    sf::Text m_text;
    sf::Font m_font;
    sf::RectangleShape m_background;
    sf::Color m_baseColor;
    sf::Color m_hoverColor;
    sf::Color m_focusColor;
    sf::Color m_pressColor;
    sf::Vector2f m_spacing;
};


#endif //LANDCRAFT_TEXTBUTTON_HPP