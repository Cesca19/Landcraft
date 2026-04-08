//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_UICONTROLLER_HPP
#define LANDCRAFT_UICONTROLLER_HPP

#include "widgets/IWidget.hpp"

class UIController {
public:
    // to do : send the window size and create a custom ui view
    UIController();
    void addWidget(std::unique_ptr<IWidget> widget);
    void update(float deltaTime, sf::RenderWindow& window);
    void handleEvents(const sf::Event &event, sf::RenderWindow& window);
    void handleContinuousEvents(float deltaTime, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

private:
    std::vector<std::unique_ptr<IWidget>> m_widgets;
    IWidget* m_focusedWidget;
};


#endif //LANDCRAFT_UICONTROLLER_HPP