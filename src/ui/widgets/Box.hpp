//
// Created by fran on 10/04/2026.
//

#ifndef LANDCRAFT_BOX_HPP
#define LANDCRAFT_BOX_HPP

#include "IWidget.hpp"
#include "RoundedRectangleShape.hpp"

class Box : public IWidget {
public:
    Box(const sf::Vector2f& position, const sf::Vector2f& size);
    void initColors(const sf::Color& backgroundColor, const sf::Color& outlineColor);

    bool isInteractable() const override;

    bool isVisible() const override;
    void setVisibility(bool isVisible) override;

    bool isSelected() const override;
    void setSelected(bool isSelected) override;

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
private:
    bool m_isVisible;
    sf::Color m_backgroundColor;
    sf::Color m_outlineColor;
    RoundedRectangleShape m_rectangle;

    // Au lieu de m_background.setSize(size);
    // m_background = RoundedRectangleShape(size, 8.0f); // 8 pixels de rayon pour l'arrondi
    // m_background.setPosition(position);
};


#endif //LANDCRAFT_BOX_HPP