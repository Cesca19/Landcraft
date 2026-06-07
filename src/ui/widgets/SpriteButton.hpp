//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_SPRITEBUTTON_HPP
#define LANDCRAFT_SPRITEBUTTON_HPP

#include <functional>
#include "IWidget.hpp"

enum class HighlightTextAlign
{
    Top,
    Down,
    Left,
    Right
};

class SpriteButton : public IWidget
{
public:
    SpriteButton(const std::string &iconPath, sf::Vector2f position, sf::Vector2f size, const std::string &highlightText, int highlightTextSize);
    void initHighLightTextColor(const sf::Color& highlightTextColor);
    void initOutlineStatesColors(const sf::Color& baseColor, const sf::Color& hoverColor, const sf::Color& focusColor, const sf::Color& pressColor, const sf::Color& selectedColor);
    void initBackgroundStatesColor(const sf::Color& baseColor, const sf::Color& hoverColor,const sf::Color& focusColor, const sf::Color& pressColor, const sf::Color& selectedColor);
    void initIconStatesColor(const sf::Color& baseColor, const sf::Color& hoverColor, const sf::Color& focusColor, const sf::Color& pressColor, const sf::Color& selectedColor);
    void initOnClickCallback(std::function<void()> callback);
    void initHighlightTextAlign(HighlightTextAlign align);

    bool isInteractable() const override;
    void setContinuousClick(bool didSupportContinuousClick, float repeatInterval = 0.25f);

    bool isVisible() const override;
    void setVisibility(bool isVisible) override;

    bool isSelected() const override;
    void setSelected(bool isSelected) override;

    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void setState(WidgetState state) override;
    void handleKeyBoardEvents(const sf::Event& event) override;
    bool shouldFocusOnClick() const override;

    void resetIconScale();
    void setPosition(const sf::Vector2f& position) override;
protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;

    sf::Sprite m_iconSprite;
    sf::Text m_highlightText;
    sf::RectangleShape m_background;

    float m_spacing;
    float m_baseThickness;
    sf::Vector2f m_padding;
    HighlightTextAlign m_highlightTextAlign;

    sf::Vector2f m_baseScale;
    sf::Vector2f m_hoverScale;
    sf::Vector2f m_pressScale;

    sf::Color m_baseColor;
    sf::Color m_hoverColor;
    sf::Color m_focusColor;
    sf::Color m_pressColor;
    sf::Color m_selectedColor;
    sf::Color m_highlightTextColor;
    sf::Color m_backgroundBaseColor;
    sf::Color m_backgroundHoverColor;
    sf::Color m_backgroundFocusColor;
    sf::Color m_backgroundPressColor;
    sf::Color m_backgroundSelectedColor;
    sf::Color m_iconBaseColor;
    sf::Color m_iconHoverColor;
    sf::Color m_iconFocusColor;
    sf::Color m_iconPressColor;
    sf::Color m_iconSelectedColor;

    bool m_isVisible;
    bool m_isSelected;
    bool m_isInteractable;
    bool m_isHighLightTextVisible;

    bool m_didSupportContinuousClick;
    float m_elapsedTimeSinceClick;
    float m_continuousClickRepeatInterval;

    WidgetState m_currentState;
    std::function<void()> m_onClickCallback;
};


#endif //LANDCRAFT_SPRITEBUTTON_HPP