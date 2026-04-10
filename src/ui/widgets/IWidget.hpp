//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_IWIDGET_HPP
#define LANDCRAFT_IWIDGET_HPP

#include <SFML/Graphics.hpp>
#include "../../utils/ResourceManager.hpp"

enum class WidgetState {
    Base,
    Hovered,
    Focused,
    Pressed
    // TO DO : add widget selected state
};

class IWidget {
public:
    virtual ~IWidget() = default;
    virtual bool isInteractable() const = 0;

    virtual bool isVisible() const = 0;
    virtual void setVisibility(bool isVisible) = 0;

    virtual bool isSelected() const = 0;
    virtual void setSelected(bool isSelected) = 0;


    virtual sf::FloatRect getBounds() const = 0;
    virtual sf::Vector2f getCenter() const = 0;

    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual void setState(WidgetState state) = 0;
protected:
    virtual void onBase() = 0;
    virtual void onHover() = 0;
    virtual void onFocus() = 0;
    virtual void onPress() = 0;
};

#endif //LANDCRAFT_IWIDGET_HPP