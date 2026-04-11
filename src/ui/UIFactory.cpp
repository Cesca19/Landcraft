//
// Created by fran on 08/04/2026.
//

#include "UIFactory.hpp"

UIController* UIFactory::s_uiController = nullptr;

void UIFactory::init(UIController *uiController)
{
    s_uiController = uiController;
}

void UIFactory::removeWidget(IWidget *widget)
{
    if (!s_uiController) {
        std::cerr << "UIFactory not initialized with an UIController" << std::endl;
        return;
    }
    s_uiController->removeWidget(widget);
}

Box *UIFactory::createBox(const sf::Vector2f &position, const sf::Vector2f &size)
{
    if (!s_uiController) {
        std::cerr << "UIFactory not initialized with an UIController" << std::endl;
        return nullptr;
    }

    std::unique_ptr<Box> box = std::make_unique<Box>(position, size);
    Box* boxPtr = box.get();
    s_uiController->addWidget(std::move(box));
    return boxPtr;
}

Text * UIFactory::createText(const sf::Vector2f& position, const std::string &content, int characterSize)
{
    if (!s_uiController) {
        std::cerr << "UIFactory not initialized with an UIController" << std::endl;
        return nullptr;
    }
    std::unique_ptr<Text> text = std::make_unique<Text>(position, content, characterSize);
    Text* textPtr = text.get();
    s_uiController->addWidget(std::move(text));
    return textPtr;
}

TextButton *UIFactory::createTextButton(sf::Vector2f position, const std::string &text, sf::Color textColor, unsigned int characterSize)
{
    if (!s_uiController) {
        std::cerr << "UIFactory not initialized with an UIController" << std::endl;
        return nullptr;
    }

    std::unique_ptr<TextButton> button = std::make_unique<TextButton>(position, text, textColor, characterSize);
    TextButton* buttonPtr = button.get();
    s_uiController->addWidget(std::move(button));
    return buttonPtr;
}

SpriteButton * UIFactory::createSpriteButton(const std::string &iconPath, sf::Vector2f position, sf::Vector2f size,
    const std::string &highlightText, int highlightTextSize)
{
    if (!s_uiController) {
        std::cerr << "UIFactory not initialized with an UIController" << std::endl;
        return nullptr;
    }

    std::unique_ptr<SpriteButton> button = std::make_unique<SpriteButton>(iconPath, position, size, highlightText, highlightTextSize);
    SpriteButton* buttonPtr = button.get();
    s_uiController->addWidget(std::move(button));
    return buttonPtr;

}
