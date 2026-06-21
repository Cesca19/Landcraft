//
// Created by fran on 17/06/2026.
//

#ifndef LANDCRAFT_MENUACTIONBUTTON_HPP
#define LANDCRAFT_MENUACTIONBUTTON_HPP

#include <functional>
#include "IWidget.hpp"
#include "RoundedRectangleShape.hpp"

class MenuActionButton : public IWidget {
public:
    MenuActionButton(sf::Vector2f position, sf::Vector2f size, const std::string &iconPath,
        const std::string &title, const std::string &subtitle, unsigned int titleSize = 22,
        unsigned int subtitleSize = 15);
    void initOutlineStatesColors(const sf::Color &baseColor, const sf::Color &hoverColor,
        const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &selectedColor);
    void initBackgroundStatesColor(const sf::Color &baseColor, const sf::Color &hoverColor,
        const sf::Color &focusColor, const sf::Color &pressColor, const sf::Color &selectedColor);
    void initTextColors(const sf::Color &titleBaseColor, const sf::Color &titleHoverColor,
        const sf::Color &subtitleBaseColor, const sf::Color &subtitleHoverColor);
    void initIconColors(const sf::Color &baseColor, const sf::Color &hoverColor);
    void initOnClickCallback(std::function<void()> callback);

    bool isInteractable() const override;
    bool isVisible() const override;
    void setVisibility(bool isVisible) override;
    bool isSelected() const override;
    void setSelected(bool isSelected) override;
    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow &window) const override;
    void setState(WidgetState state) override;
    void handleKeyBoardEvents(const sf::Event &event) override;
    bool shouldFocusOnClick() const override;
    void setPosition(const sf::Vector2f &position) override;
    void setSize(const sf::Vector2f &size);
protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;
    void refreshLayout();

    bool m_isVisible;
    bool m_isSelected;
    bool m_isInteractable;
    WidgetState m_currentState;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    RoundedRectangleShape m_background;
    sf::Sprite m_iconSprite;
    sf::Text m_title;
    sf::Text m_subtitle;
    sf::Color m_baseOutlineColor;
    sf::Color m_hoverOutlineColor;
    sf::Color m_focusOutlineColor;
    sf::Color m_pressOutlineColor;
    sf::Color m_selectedOutlineColor;
    sf::Color m_baseBackgroundColor;
    sf::Color m_hoverBackgroundColor;
    sf::Color m_focusBackgroundColor;
    sf::Color m_pressBackgroundColor;
    sf::Color m_selectedBackgroundColor;
    sf::Color m_titleBaseColor;
    sf::Color m_titleHoverColor;
    sf::Color m_subtitleBaseColor;
    sf::Color m_subtitleHoverColor;
    sf::Color m_iconBaseColor;
    sf::Color m_iconHoverColor;
    std::function<void()> m_onClickCallback;
};

#endif //LANDCRAFT_MENUACTIONBUTTON_HPP
