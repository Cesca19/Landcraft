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
    enum class TextVariant {
        Default,
        Title,
        Underlined,
        Label,
        Value
    };

    static void init(UIController* uiController);
    static void removeWidget(IWidget* widget);
    static Box* createBox(const sf::Vector2f& position, const sf::Vector2f& size);
    static Text *createText(const sf::Vector2f& position, const std::string& content, int characterSize);
    static TextButton* createTextButton(sf::Vector2f position, const std::string& text, unsigned int characterSize);
    static SpriteButton* createSpriteButton(const std::string &iconPath, sf::Vector2f position, sf::Vector2f size, const std::string &highlightText, int highlightTextSize);
    
    // Default style functions
    static void applyDefaultTextStyle(Text* text, TextVariant variant = TextVariant::Default);
    static void applyDefaultSpriteButtonStyle(SpriteButton* button, HighlightTextAlign align = HighlightTextAlign::Top, bool shouldInitIcon = true);
    static void applyDefaultTextButtonStyle(TextButton* button, TextVariant textVariant = TextVariant::Default);
    static void applyDefaultBoxStyle(Box* box);
private:
    static UIController* s_uiController;
};


#endif //LANDCRAFT_UIFACTORY_HPP