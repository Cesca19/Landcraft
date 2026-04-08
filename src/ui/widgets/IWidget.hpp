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
};

class IWidget {
public:
    virtual ~IWidget() = default;
    virtual bool isInteractable() const = 0;

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