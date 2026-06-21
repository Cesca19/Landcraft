//
// Created by fran on 17/06/2026.
//

#ifndef LANDCRAFT_IMAGE_HPP
#define LANDCRAFT_IMAGE_HPP

#include "IWidget.hpp"

class Image : public IWidget {
public:
    Image(const std::string &texturePath, sf::Vector2f position,
        sf::Vector2f displaySize = {0.f, 0.f});
    void setTexture(const std::string &texturePath);
    void setDisplaySize(const sf::Vector2f &displaySize);
    void setScale(const sf::Vector2f &scale);
    void setColor(const sf::Color &color);
    void setRotation(float angleDegrees);
    void setOrigin(const sf::Vector2f &origin);

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
    void setPosition(const sf::Vector2f &position) override;
    bool shouldFocusOnClick() const override;
protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;
private:
    void applyDisplaySize();

    bool m_isVisible;
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
    sf::Vector2f m_displaySize;
    bool m_useDisplaySize;
};

#endif //LANDCRAFT_IMAGE_HPP
