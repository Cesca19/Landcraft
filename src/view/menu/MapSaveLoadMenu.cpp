//
// Created by fran on 06/05/2026.
//

#include "MapSaveLoadMenu.hpp"

MapSaveLoadMenu::MapSaveLoadMenu(sf::Vector2f globalUIPosition)
    : m_globalUIStartPosition(globalUIPosition)
{
    m_menuBox = UIFactory::createBox(globalUIPosition, {205, 90});
    UIFactory::applyDefaultBoxStyle(m_menuBox);
    m_menuTitle = UIFactory::createText(globalUIPosition + sf::Vector2f(10, 35), "Map", 15);
    UIFactory::applyDefaultTextStyle(m_menuTitle, UIFactory::TextVariant::Label);

    sf::Vector2f buttonPosition = globalUIPosition + sf::Vector2f(50, 0);
    m_saveFileButton = UIFactory::createSpriteButton("assets/textures/ui/download_512.png",
        buttonPosition + sf::Vector2f(15, 20), {28, 28}, "Save", 12);
    m_loadFileButton = UIFactory::createSpriteButton("assets/textures/ui/upload_512.png",
        buttonPosition + sf::Vector2f(85, 20), {28, 28}, "Load", 12);
     UIFactory::applyDefaultSpriteButtonStyle(m_saveFileButton, HighlightTextAlign::Down);
     UIFactory::applyDefaultSpriteButtonStyle(m_loadFileButton, HighlightTextAlign::Down);
    initWidgetsList();
}

MapSaveLoadMenu::~MapSaveLoadMenu()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void MapSaveLoadMenu::setSaveFileButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_saveFileButton->initOnClickCallback(callback);
}

void MapSaveLoadMenu::setLoadFileButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_loadFileButton->initOnClickCallback(callback);
}

void MapSaveLoadMenu::setVisibility(bool visible) const
{
    for (auto *widget: m_widgets) {
        widget->setVisibility(visible);
    }
}

void MapSaveLoadMenu::initWidgetsList()
{
    m_widgets.push_back(m_menuBox);
    m_widgets.push_back(m_menuTitle);
    m_widgets.push_back(m_saveFileButton);
    m_widgets.push_back(m_loadFileButton);
}
