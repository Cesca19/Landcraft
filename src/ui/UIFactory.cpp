//
// Created by fran on 08/04/2026.
//

#include <stdexcept>
#include "UIFactory.hpp"

UIController* UIFactory::s_uiController = nullptr;

void UIFactory::init(UIController *uiController)
{
    s_uiController = uiController;
}

void UIFactory::removeWidget(IWidget *widget)
{
    if (!s_uiController) {
        throw std::runtime_error("UIFactory not initialized with a UIController");
    }
    s_uiController->removeWidget(widget);
}

Box *UIFactory::createBox(const sf::Vector2f &position, const sf::Vector2f &size)
{
    if (!s_uiController) {
        throw std::runtime_error("UIFactory not initialized with a UIController");
    }

    std::unique_ptr<Box> box = std::make_unique<Box>(position, size);
    Box* boxPtr = box.get();
    s_uiController->addWidget(std::move(box));
    return boxPtr;
}

Text * UIFactory::createText(const sf::Vector2f& position, const std::string &content, int characterSize)
{
    if (!s_uiController) {
        throw std::runtime_error("UIFactory not initialized with a UIController");
    }
    std::unique_ptr<Text> text = std::make_unique<Text>(position, content, characterSize);
    Text* textPtr = text.get();
    s_uiController->addWidget(std::move(text));
    return textPtr;
}

TextInput *UIFactory::createTextInput(sf::Vector2f position, sf::Vector2f size, const std::string &placeholder, 
    unsigned int characterSize, bool isNumeric)
{
    if (!s_uiController) {
        throw std::runtime_error("UIFactory not initialized with a UIController");
    }
    std::unique_ptr<TextInput> textInput = std::make_unique<TextInput>(position, size, placeholder, characterSize, isNumeric);
    TextInput* textInputPtr = textInput.get();
    s_uiController->addWidget(std::move(textInput));
    return textInputPtr;
}

TextButton *UIFactory::createTextButton(sf::Vector2f position, const std::string &text, unsigned int characterSize)
{
    if (!s_uiController) {
        throw std::runtime_error("UIFactory not initialized with a UIController");
    }

    std::unique_ptr<TextButton> button = std::make_unique<TextButton>(position, text, characterSize);
    TextButton* buttonPtr = button.get();
    s_uiController->addWidget(std::move(button));
    return buttonPtr;
}

SpriteButton * UIFactory::createSpriteButton(const std::string &iconPath, sf::Vector2f position, sf::Vector2f size,
    const std::string &highlightText, int highlightTextSize)
{
    if (!s_uiController) {
        throw std::runtime_error("UIFactory not initialized with a UIController");
    }

    std::unique_ptr<SpriteButton> button = std::make_unique<SpriteButton>(iconPath, position, size, highlightText, highlightTextSize);
    SpriteButton* buttonPtr = button.get();
    s_uiController->addWidget(std::move(button));
    return buttonPtr;

}

void UIFactory::applyDefaultTextStyle(Text* text, const TextVariant variant)
{
    if (!text) return;

    switch (variant) {
        case TextVariant::Title:
            text->init(sf::Color(100, 80, 150), sf::Text::Bold | sf::Text::Underlined);
            break;
        case TextVariant::Underlined:
            text->init(sf::Color(100, 80, 150), sf::Text::Underlined);
            break;
        case TextVariant::Label:
            text->init(sf::Color(110, 95, 150), sf::Text::Bold | sf::Text::Italic);
            break;
        case TextVariant::Value:
            text->init(sf::Color(90, 70, 130), sf::Text::Bold);
            break;
        case TextVariant::Default:
        default:
            text->init(sf::Color(110, 95, 150), sf::Text::Regular);
            break;
    }
}

void UIFactory::applyDefaultSpriteButtonStyle(SpriteButton* button, const HighlightTextAlign align, const bool shouldInitIcon)
{
    if (!button) return;
    button->initOutlineStatesColors(
        sf::Color(220, 210, 240),  // normal
        sf::Color(180, 150, 230),  // hover (VISIBLE)
        sf::Color(160, 120, 220),  // focus
        sf::Color(130, 95, 185),   // pressed
        sf::Color(160, 120, 220)   // selected
    );
    button->initBackgroundStatesColor(
        sf::Color(248, 246, 252),  // normal
        sf::Color(235, 225, 250),  // hover
        sf::Color(235, 225, 250),  // focus
        sf::Color(210, 190, 240),  // pressed
        sf::Color(235, 225, 250)
    );
    button->initHighLightTextColor(sf::Color(90, 70, 130));
    button->initHighlightTextAlign(align);
    if (shouldInitIcon)
        button->initIconStatesColor(
            sf::Color(110, 95, 150),   // normal
            sf::Color(140, 110, 200),  // hover
            sf::Color(140, 110, 200),  // focus
            sf::Color(110, 80, 170),   // pressed
            sf::Color(110, 95, 150)    // selected
        );
}

void UIFactory::applyDefaultTextButtonStyle(TextButton *button, TextVariant textVariant)
{
    if (!button) 
        return;

    button->initOutlineStatesColors(
        sf::Color(220, 210, 240),  // normal
        sf::Color(180, 150, 230),  // hover (VISIBLE)
        sf::Color(160, 120, 220),  // focus 
        sf::Color(130, 95, 185),   // pressed
        sf::Color(160, 120, 220)   // selected  
    );
    button->initBackgroundStatesColor(
        sf::Color(248, 246, 252),  // normal
        sf::Color(235, 225, 250),  // hover
        sf::Color(235, 225, 250),  // focus
        sf::Color(210, 190, 240),  // pressed
        sf::Color(235, 225, 250)   // selected
    );
    button->initTextColor(
        sf::Color(110, 95, 150),   // normal
        sf::Color(140, 110, 200),  // hover
        sf::Color(140, 110, 200),  // focus
        sf::Color(110, 80, 170),   // pressed
        sf::Color(110, 95, 150)    // selected
    );
}

void UIFactory::applyDefaultTextInputStyle(TextInput *textInput)
{
    textInput->initBackgroundColors(
        sf::Color(248, 246, 252),  // normal
        sf::Color(235, 225, 250),  // hover
        sf::Color(235, 225, 250),  // focus
        sf::Color(210, 190, 240)
    );

    textInput->initOutlineColors(
        sf::Color(220, 210, 240),  // normal
        sf::Color(180, 150, 230),  // hover
        sf::Color(160, 120, 220),  // focus 
        sf::Color(130, 95, 185)
    );

    textInput->initTextColors(
        sf::Color(110, 95, 150),   // normal
        sf::Color(140, 110, 200),  // hover
        sf::Color(140, 110, 200),  // focus
        sf::Color(110, 80, 170)
    );
}

void UIFactory::applyDefaultBoxStyle(Box* box)
{
    if (!box) return;
    box->initColors(
        sf::Color(255, 255, 255),
        sf::Color(220, 210, 240)
    );
}
