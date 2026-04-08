//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_UICONTROLLER_HPP
#define LANDCRAFT_UICONTROLLER_HPP

#include "widgets/IWidget.hpp"
#include <memory>
#include <vector>

class UIController {
public:
    // to do : send the window size and create a custom ui view
    UIController();
    void addWidget(std::unique_ptr<IWidget> widget);
    void handleEvents(const sf::Event &event, const sf::RenderWindow& window);
    void handleContinuousEvents(float deltaTime, const sf::RenderWindow& window);
    void update(float deltaTime, sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window) const;
    // to do : add on window resize function

private:
    void findHoveredWidget(const sf::RenderWindow& window);
    static sf::Vector2f getMouseScreenPosition(const sf::RenderWindow& window);

    std::vector<std::unique_ptr<IWidget>> m_widgets;
    IWidget* m_focusedWidget;
    IWidget* m_hoveredWidget;
};


#endif //LANDCRAFT_UICONTROLLER_HPP