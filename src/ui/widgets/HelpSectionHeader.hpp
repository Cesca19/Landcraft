//
// Created by fran on 17/06/2026.
//

#ifndef LANDCRAFT_HELPSECTIONHEADER_HPP
#define LANDCRAFT_HELPSECTIONHEADER_HPP

#include <string>
#include <algorithm>
#include "IWidget.hpp"

class HelpSectionHeader : public IWidget {
public:
    HelpSectionHeader(const sf::Vector2f &position, const std::string &iconPath,
        const std::string &title, unsigned int titleSize = 20);
    void initIconColor(const sf::Color &color);
    void initTitleColor(const sf::Color &color);

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

    bool m_isVisible;
    sf::Vector2f m_position;
    sf::Sprite m_iconSprite;
    sf::Text m_title;
    sf::Color m_iconColor;
};

#endif //LANDCRAFT_HELPSECTIONHEADER_HPP
