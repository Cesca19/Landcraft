//
// Created by fran on 15/04/2026.
//

#include "BrushMenu.hpp"

BrushMenu::BrushMenu(sf::Vector2f brushSizeUIStartPosition, const std::vector<std::string> &brushImagePaths)
{
    initBrushSizeWidgets(brushSizeUIStartPosition);
    initBrushTypeWidgets(brushImagePaths);
    initWidgetsList();
}

BrushMenu::~BrushMenu()
{
    for (auto *widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void BrushMenu::setIncrementBrushSizeButtonCallback(std::function<void()> callback)
{
    m_incrementBrushSize->initOnClickCallback(callback);
}

void BrushMenu::setDecrementBrushSizeButtonCallback(std::function<void()> callback)
{
    m_decrementBrushSize->initOnClickCallback(callback);
}

void BrushMenu::setBrushSizeValueText(const std::string &value)
{
    m_brushSizeValueText->setContent(value);
}

void BrushMenu::setBrushTypeButtonCallback(int index, std::function<void()> callback)
{
    if (index >= 0 && index < m_brushTypesButtons.size()) {
        m_brushTypesButtons[index]->initOnClickCallback(callback);
    }
}

void BrushMenu::initBrushTypeWidgets(const std::vector<std::string> &brushImagePaths)
{
    sf::Vector2f menuPosition = m_brushSizeUIStartPosition + sf::Vector2f(1780, 150);
    m_brushTypeBox = UIFactory::createBox(menuPosition, {110, 50 + static_cast<float>(brushImagePaths.size()) * 75});
    m_brushTypeBox->initColors(sf::Color(205, 185, 220), sf::Color(255, 255, 255));
    m_brushTypeText = UIFactory::createText(menuPosition + sf::Vector2f(10, 10), "Brushes:", 20);
    m_brushTypeText->init(sf::Color(123, 101, 81), sf::Text::Bold | sf::Text::Underlined);

    sf::Vector2f buttonPosition = menuPosition + sf::Vector2f(30, 50);
    for (const auto& path : brushImagePaths) {
        auto *button = UIFactory::createSpriteButton(path, buttonPosition, sf::Vector2f(32, 32), "", 12);
        m_brushTypesButtons.push_back(button);
        buttonPosition.y += 75;
        // initButtonStyle(button);
        button->initIconStatesColor(
            sf::Color(123, 101, 81),
            sf::Color(255, 245, 196),
            sf::Color(255, 236, 168),
            sf::Color(255, 255, 220)
        );
    }
}

void BrushMenu::initBrushSizeWidgets(sf::Vector2f brushSizeUIStartPosition)
{
    m_brushSizeBox = UIFactory::createBox(brushSizeUIStartPosition, {280, 80});
    m_brushSizeBox->initColors(sf::Color(205, 185, 220), sf::Color(255, 255, 255));
    m_brushSizeText = UIFactory::createText( brushSizeUIStartPosition + sf::Vector2f(10, 30), "Brush size", 15);
    m_brushSizeText->init(sf::Color(123, 101, 81), sf::Text::Bold | sf::Text::Italic);

    const sf::Vector2f startButtonPosition = brushSizeUIStartPosition + sf::Vector2f(40, 0);

    m_brushSizeValueText = UIFactory::createText(startButtonPosition + sf::Vector2f(130, 27), "", 20);
    m_brushSizeValueText->init(sf::Color::White, sf::Text::Bold);

    m_decrementBrushSize = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png", startButtonPosition + sf::Vector2f(70, 15),
        sf::Vector2f(28, 28), "Reduce", 12);
    m_incrementBrushSize = UIFactory::createSpriteButton("assets/textures/ui/add_512.png", startButtonPosition + sf::Vector2f(170, 15),
        sf::Vector2f(28, 28), "Add", 12);

    initButtonStyle(m_decrementBrushSize, HighlightTextAlign::Down);
    initButtonStyle(m_incrementBrushSize, HighlightTextAlign::Down);
}

void BrushMenu::initButtonStyle(SpriteButton *button, HighlightTextAlign align)
{
    button->initOutlineStatesColors(sf::Color(255, 255, 255, 175), sf::Color(178, 247, 239),
        sf::Color(115, 80, 135), sf::Color(255, 255, 255, 225), sf::Color(123, 101, 81));
    button->initBackgroundStatesColor(sf::Color(253, 247, 216), sf::Color(255, 240, 180),
        sf::Color(250, 239, 250), sf::Color(253, 249, 221));
    button->initHighlightTextAlign(align);
}

void BrushMenu::initWidgetsList()
{
    m_widgets.push_back(m_brushSizeBox);
    m_widgets.push_back(m_brushSizeText);
    m_widgets.push_back(m_incrementBrushSize);
    m_widgets.push_back(m_decrementBrushSize);
    m_widgets.push_back(m_brushSizeValueText);
    m_widgets.push_back(m_brushTypeBox);
    m_widgets.push_back(m_brushTypeText);
    m_widgets.insert(m_widgets.end(), m_brushTypesButtons.begin(), m_brushTypesButtons.end());
}
