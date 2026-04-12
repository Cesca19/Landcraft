//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_UICONTROLLER_HPP
#define LANDCRAFT_UICONTROLLER_HPP

#include <functional>
#include <memory>
#include <vector>
#include <limits>
#include "widgets/IWidget.hpp"
#include "../utils/MathUtils.hpp"


class UIController {
public:
    // to do : send the window size and create a custom ui view
    UIController();
    ~UIController();
    void addWidget(std::unique_ptr<IWidget> widget);
    void removeWidget(IWidget* widgetToRemove);
    void handleEvents(const sf::Event &event, const sf::RenderWindow& window);
    void handleContinuousEvents(float deltaTime, const sf::RenderWindow& window);
    void update(float deltaTime, sf::RenderWindow& window) const;
    void draw(sf::RenderWindow& window) const;
    void setOnDestroy(std::function<void()> onDestroy);
    // to do : add on window resize function
    bool isUserOverUI() const;
    bool isMouseHoverUI() const;
    bool isKeyBoardNavigatingHoverUI() const;
private:
    void handleMouseEvents(const sf::Event &event, const sf::RenderWindow& window);
    void handleKeyBoardEvents(const sf::Event &event);
    void findHoveredWidget(const sf::RenderWindow& window);
    static sf::Vector2f getMouseScreenPosition(const sf::RenderWindow& window);

    void spatialNavigation(sf::Keyboard::Key key);
    IWidget* findClosestWidgetAlongAxis(sf::Vector2f mainAxis, sf::Vector2f orthogonalAxis, sf::Vector2f widgetCenter) const;
    void navigateSequentially(bool forward);
    void focusOnDefaultWidget();
    void unfocusCurrentWidget();

    bool m_isMouseHoverUI;
    std::function<void()> m_onDestroy;
    std::vector<std::unique_ptr<IWidget>> m_widgets;
    IWidget* m_focusedWidget;
    IWidget* m_hoveredWidget;
};


#endif //LANDCRAFT_UICONTROLLER_HPP