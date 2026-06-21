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
    static TextInput* createTextInput(sf::Vector2f position, sf::Vector2f size, const std::string& placeholder,
         unsigned int characterSize, bool isNumeric = false);
    static TextButton* createTextButton(sf::Vector2f position, const std::string& text, unsigned int characterSize);
    static MenuActionButton* createMenuActionButton(sf::Vector2f position, sf::Vector2f size,
        const std::string& iconPath, const std::string& title, const std::string& subtitle,
        unsigned int titleSize = 22, unsigned int subtitleSize = 15);
    static SpriteButton* createSpriteButton(const std::string &iconPath, sf::Vector2f position, sf::Vector2f size, const std::string &highlightText, int highlightTextSize);
    static Image* createImage(const std::string &texturePath, sf::Vector2f position,
        sf::Vector2f displaySize = {0.f, 0.f});
    static Keycap* createKeycap(sf::Vector2f position, const std::string &label,
        unsigned int characterSize = 13);
    static HelpSectionHeader* createHelpSectionHeader(sf::Vector2f position,
        const std::string &iconPath, const std::string &title,
        unsigned int titleSize = 30);
    static HelpControlRow* createHelpControlRow(sf::Vector2f position, sf::Vector2f size,
        const std::string &iconPath, const std::string &title, const std::string &subtitle,
        const std::vector<HelpControlRow::MouseHint> &mouseHints = {},
        const std::vector<std::vector<std::string>> &shortcutGroups = {},
        const std::vector<HelpControlRow::LegendItem> &legendItems = {});
    
    // Default style functions
    static void applyDefaultTextStyle(Text* text, TextVariant variant = TextVariant::Default);
    static void applyDefaultSpriteButtonStyle(SpriteButton* button, HighlightTextAlign align = HighlightTextAlign::Top, bool shouldInitIcon = true);
    static void applyDefaultTextButtonStyle(TextButton* button, TextVariant textVariant = TextVariant::Default);
    static void applyDefaultMenuActionButtonStyle(MenuActionButton* button);
    static void applyDefaultKeycapStyle(Keycap* keycap);
    static void applyDefaultHelpSectionHeaderStyle(HelpSectionHeader* header);
    static void applyDefaultTextInputStyle(TextInput* textInput);
    static void applyDefaultBoxStyle(Box* box);
private:
    static UIController* s_uiController;
};


#endif //LANDCRAFT_UIFACTORY_HPP