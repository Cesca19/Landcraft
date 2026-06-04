//
// Created by fran on 02/06/2026.
//

#include "TerrainGenerationMenu.hpp"

TerrainGenerationMenu::TerrainGenerationMenu(const sf::Vector2f &terrainGenerationMenuPosition, const sf::Vector2u &windowSize)
{
    m_menuBox = UIFactory::createBox(terrainGenerationMenuPosition, sf::Vector2f(200, 90));
    UIFactory::applyDefaultBoxStyle(m_menuBox);

    m_MenuTitle = UIFactory::createText(terrainGenerationMenuPosition + sf::Vector2f(20, 25), "Terrain\nGenerator", 15);
    UIFactory::applyDefaultTextStyle(m_MenuTitle, UIFactory::TextVariant::Label);

    m_TerrainGenerationMenuButton = UIFactory::createSpriteButton("assets/textures/ui/terrain_generator_512.png", 
        terrainGenerationMenuPosition + sf::Vector2f(130, 20), {28, 28}, "Show Menu", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_TerrainGenerationMenuButton, HighlightTextAlign::Down);

    initTerrainGenerationMenu(windowSize);
    setTerrainGenerationMenuVisibility(false);
    initWidgetsList();
}

TerrainGenerationMenu::~TerrainGenerationMenu()
{
    for (auto *widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void TerrainGenerationMenu::initOnTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback)
{
    m_TerrainGenerationMenuButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnCloseTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback)
{
    m_closeTerrainGenerationMenuButton->initOnClickCallback(callback);
    m_cancelButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnGenerateButtonClickCallback(const std::function<void()> &callback)
{
    m_generateButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnSeedInputValidatedCallback(const std::function<void(const std::string &)> &callback)
{
    m_seedInput->initOnValidateCallback(callback);
}

void TerrainGenerationMenu::setTerrainGenerationMenuVisibility(bool isVisible)
{
    for (auto *widget : m_menuWidgets)
        widget->setVisibility(isVisible);
}

void TerrainGenerationMenu::initTerrainGenerationMenu(const sf::Vector2u &windowSize)
{
    sf::Vector2f menuSize = sf::Vector2f(400, 350);
    sf::Vector2f menuPosition = sf::Vector2f(windowSize.x / 2.0f - menuSize.x / 2.0f, windowSize.y / 2.0f - menuSize.y / 2.0f);
    m_TerrainGenerationMenuBox = UIFactory::createBox(menuPosition, menuSize);
    UIFactory::applyDefaultBoxStyle(m_TerrainGenerationMenuBox);

    m_closeTerrainGenerationMenuButton = UIFactory::createSpriteButton("assets/textures/ui/close_512.png", 
        menuPosition + sf::Vector2f(menuSize.x - 50, 10), {15, 15}, "Close", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_closeTerrainGenerationMenuButton, HighlightTextAlign::Down);

    m_TerrainGenerationMenuTitle = UIFactory::createText(menuPosition + sf::Vector2f(90, 20), "Terrain Generation", 18);
    UIFactory::applyDefaultTextStyle(m_TerrainGenerationMenuTitle, UIFactory::TextVariant::Title);

    m_seedText = UIFactory::createText(menuPosition + sf::Vector2f(20, 70), "Seed: ", 15);
    UIFactory::applyDefaultTextStyle(m_seedText, UIFactory::TextVariant::Value);
    m_frequencyText = UIFactory::createText(menuPosition + sf::Vector2f(20, 120), "Frequency: ", 15);
    UIFactory::applyDefaultTextStyle(m_frequencyText, UIFactory::TextVariant::Value);
    m_octavesText = UIFactory::createText(menuPosition + sf::Vector2f(20, 170), "Octaves: ", 15);
    UIFactory::applyDefaultTextStyle(m_octavesText, UIFactory::TextVariant::Value);
    m_ExponentText = UIFactory::createText(menuPosition + sf::Vector2f(20, 220), "Exponent: ", 15);
    UIFactory::applyDefaultTextStyle(m_ExponentText, UIFactory::TextVariant::Value);

    m_generateButton = UIFactory::createTextButton(menuPosition + sf::Vector2f(40, 270), "Generate", 17);
    UIFactory::applyDefaultTextButtonStyle(m_generateButton, UIFactory::TextVariant::Default);
    m_cancelButton = UIFactory::createTextButton(menuPosition + sf::Vector2f(200, 270), "Cancel", 17);
    UIFactory::applyDefaultTextButtonStyle(m_cancelButton, UIFactory::TextVariant::Default);

    m_seedInput = UIFactory::createTextInput(menuPosition + sf::Vector2f(170, 60), sf::Vector2f(160, 40), "Enter seed (Ex: 1337)", 18, true);
    m_seedInput->initCharacterLimit(10);
    UIFactory::applyDefaultTextInputStyle(m_seedInput);

    m_randomSeedButton = UIFactory::createSpriteButton("assets/textures/ui/random_512.png", 
        menuPosition + sf::Vector2f(345, 60), {18, 18}, "Random", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_randomSeedButton, HighlightTextAlign::Down);

    m_menuWidgets.push_back(m_TerrainGenerationMenuBox);
    m_menuWidgets.push_back(m_closeTerrainGenerationMenuButton);
    m_menuWidgets.push_back(m_TerrainGenerationMenuTitle);
    m_menuWidgets.push_back(m_seedText);
    m_menuWidgets.push_back(m_frequencyText);
    m_menuWidgets.push_back(m_octavesText);
    m_menuWidgets.push_back(m_ExponentText);
    m_menuWidgets.push_back(m_generateButton);
    m_menuWidgets.push_back(m_cancelButton);
    m_menuWidgets.push_back(m_seedInput);
    m_menuWidgets.push_back(m_randomSeedButton);
}

void TerrainGenerationMenu::initWidgetsList()
{
    m_widgets.push_back(m_menuBox);
    m_widgets.push_back(m_MenuTitle);
    m_widgets.push_back(m_TerrainGenerationMenuButton);
    for (auto *widget : m_menuWidgets)
        m_widgets.push_back(widget);
}
