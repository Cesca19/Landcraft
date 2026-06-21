//
// Created by fran on 17/06/2026.
//

#ifndef LANDCRAFT_KEYCAP_HPP
#define LANDCRAFT_KEYCAP_HPP

#include "IWidget.hpp"
#include "RoundedRectangleShape.hpp"

class Keycap : public IWidget {
public:
    Keycap(const sf::Vector2f &position, const std::string &label,
        unsigned int characterSize = 13);
    void initColors(const sf::Color &backgroundColor, const sf::Color &outlineColor,
        const sf::Color &textColor);
    void setLabel(const std::string &label);

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

protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;

private:
    void refreshLayout();

    static constexpr float s_minWidth = 22.f;
    static constexpr float s_height = 24.f;
    static constexpr float s_horizontalPadding = 8.f;
    static constexpr float s_cornerRadius = 6.f;

    bool m_isVisible;
    sf::Vector2f m_position;
    sf::Color m_backgroundColor;
    sf::Color m_outlineColor;
    sf::Color m_textColor;
    RoundedRectangleShape m_background;
    sf::Text m_label;
};

#endif //LANDCRAFT_KEYCAP_HPP
