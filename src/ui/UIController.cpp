//
// Created by fran on 08/04/2026.
//

#include "UIController.hpp"

UIController::UIController()
{
}

void UIController::addWidget(std::unique_ptr<IWidget> widget)
{
    m_widgets.push_back(std::move(widget));
}

void UIController::draw(sf::RenderWindow &window) const
{
    sf::View lastView = window.getView();

    window.setView(window.getDefaultView());
    for (const auto& widget : m_widgets)
        widget->draw(window);
    window.setView(lastView);
}
