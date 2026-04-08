//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_UIFACTORY_HPP
#define LANDCRAFT_UIFACTORY_HPP

#include <iostream>
#include <memory>
#include "widgets/Widgets.hpp"
#include "UIController.hpp"

class UIFactory {
    public:
    static void init(UIController* uiController);
    static TextButton* createTextButton(sf::Vector2f position, const std::string& text, sf::Color textColor, unsigned int characterSize);
private:
    static UIController* s_uiController;
};


#endif //LANDCRAFT_UIFACTORY_HPP