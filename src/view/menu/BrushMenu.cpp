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
    for (auto *widget: m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void BrushMenu::setIncrementBrushSizeButtonCallback(const std::function<void()> &callback) const
{
    m_incrementBrushSize->initOnClickCallback(callback);
}

void BrushMenu::setDecrementBrushSizeButtonCallback(const std::function<void()> &callback) const
{
    m_decrementBrushSize->initOnClickCallback(callback);
}

void BrushMenu::setBrushSizeValueText(const std::string &value) const
{
    m_brushSizeValueText->setContent(value);
}

void BrushMenu::setBrushTypeButtonCallback(const int index, const std::function<void()> &callback) const
{
    if (index >= 0 && index < m_brushTypesButtons.size()) {
        m_brushTypesButtons[index]->initOnClickCallback(callback);
    }
}

void BrushMenu::selectBrush(const int index) const
{
    m_brushTypesButtons[index]->setSelected(true);
}

void BrushMenu::unselectBrush(const int index) const
{
    m_brushTypesButtons[index]->setSelected(false);
}

void BrushMenu::setVisibility(bool visible) const
{
    for (auto *widget: m_widgets) {
        widget->setVisibility(visible);
    }
}

void BrushMenu::initBrushTypeWidgets(const std::vector<std::string> &brushImagePaths)
{
    sf::Vector2f menuPosition = m_brushSizeUIStartPosition + sf::Vector2f(1800, 150);
    m_brushTypeBox = UIFactory::createBox(menuPosition, {110, 50 + static_cast<float>(brushImagePaths.size()) * 75});
    UIFactory::applyDefaultBoxStyle(m_brushTypeBox);
    m_brushTypeText = UIFactory::createText(menuPosition + sf::Vector2f(10, 10), "Brushes:", 20);
    UIFactory::applyDefaultTextStyle(m_brushTypeText, UIFactory::TextVariant::Title);

    sf::Vector2f buttonPosition = menuPosition + sf::Vector2f(30, 50);
    for (const auto &path: brushImagePaths) {
        auto *button = UIFactory::createSpriteButton(path, buttonPosition, sf::Vector2f(32, 32), "", 12);
        m_brushTypesButtons.push_back(button);
        buttonPosition.y += 75;
        UIFactory::applyDefaultSpriteButtonStyle(button);
        // button->initIconStatesColor(sf::Color(123, 101, 81),sf::Color(123, 101, 81),
        //     sf::Color(123, 101, 81),sf::Color(123, 101, 81)
        // );
    }
    m_brushTypesButtons[0]->resetIconScale();
}

void BrushMenu::initBrushSizeWidgets(const sf::Vector2f brushSizeUIStartPosition) {
    m_brushSizeBox = UIFactory::createBox(brushSizeUIStartPosition, {280, 90});
    UIFactory::applyDefaultBoxStyle(m_brushSizeBox);
    m_brushSizeText = UIFactory::createText(brushSizeUIStartPosition + sf::Vector2f(10, 35), "Brush size", 15);
    UIFactory::applyDefaultTextStyle(m_brushSizeText, UIFactory::TextVariant::Label);

    const sf::Vector2f startButtonPosition = brushSizeUIStartPosition + sf::Vector2f(40, 5);

    m_brushSizeValueText = UIFactory::createText(startButtonPosition + sf::Vector2f(130, 27), "", 20);
    UIFactory::applyDefaultTextStyle(m_brushSizeValueText, UIFactory::TextVariant::Value);

    m_decrementBrushSize = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png",
                                                         startButtonPosition + sf::Vector2f(70, 15),
                                                         sf::Vector2f(28, 28), "Reduce", 12);
    m_incrementBrushSize = UIFactory::createSpriteButton("assets/textures/ui/add_512.png",
                                                         startButtonPosition + sf::Vector2f(170, 15),
                                                         sf::Vector2f(28, 28), "Add", 12);

    UIFactory::applyDefaultSpriteButtonStyle(m_decrementBrushSize, HighlightTextAlign::Down);
    UIFactory::applyDefaultSpriteButtonStyle(m_incrementBrushSize, HighlightTextAlign::Down);
}

void BrushMenu::initWidgetsList() {
    m_widgets.push_back(m_brushSizeBox);
    m_widgets.push_back(m_brushSizeText);
    m_widgets.push_back(m_incrementBrushSize);
    m_widgets.push_back(m_decrementBrushSize);
    m_widgets.push_back(m_brushSizeValueText);
    m_widgets.push_back(m_brushTypeBox);
    m_widgets.push_back(m_brushTypeText);
    m_widgets.insert(m_widgets.end(), m_brushTypesButtons.begin(), m_brushTypesButtons.end());
}
