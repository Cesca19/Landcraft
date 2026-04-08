//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_SPRITEBUTTON_HPP
#define LANDCRAFT_SPRITEBUTTON_HPP

#include <functional>
#include "IWidget.hpp"


class SpriteButton : public IWidget
{
public:
    SpriteButton(const std::string &iconPath, sf::Vector2f position, sf::Vector2f size, const std::string &highlightText, int highlightTextSize);
    void initStatesColors(const sf::Color& baseColor, const sf::Color& hoverColor,
        const sf::Color& focusColor, const sf::Color& pressColor, const sf::Color& highlightTextColor);
    void initOnClickCallback(std::function<void()> callback);

    bool isInteractable() const override;
    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void setState(WidgetState state) override;

protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;

    bool m_isInteractable;
    WidgetState m_currentState;
    sf::Sprite m_iconSprite;
    sf::Text m_highlightText;
    sf::RectangleShape m_background;
    sf::Color m_baseColor;
    sf::Color m_hoverColor;
    sf::Color m_focusColor;
    sf::Color m_pressColor;
    sf::Color m_highlightTextColor;
    sf::Vector2f m_padding;
    float m_spacing;
    sf::Vector2f m_baseScale;
    sf::Vector2f m_hoverScale;
    sf::Vector2f m_pressScale;
    std::function<void()> m_onClickCallback;
};


#endif //LANDCRAFT_SPRITEBUTTON_HPP