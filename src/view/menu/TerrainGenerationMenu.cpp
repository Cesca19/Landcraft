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

void TerrainGenerationMenu::initOnTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback) const
{
    m_TerrainGenerationMenuButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnCloseTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback) const
{
    m_closeTerrainGenerationMenuButton->initOnClickCallback(callback);
    m_cancelButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnGenerateButtonClickCallback(const std::function<void()> &callback) const
{
    m_generateButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnSeedInputValidatedCallback(const std::function<void(const std::string &)> &callback) const
{
    m_seedInput->initOnValidateCallback(callback);
}

void TerrainGenerationMenu::initOnRandomSeedButtonClickCallback(const std::function<void()> &callback) const
{
    m_randomSeedButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnFrequencyIncreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_frequencyIncreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnFrequencyDecreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_frequencyDecreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnOctavesIncreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_octavesIncreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnOctavesDecreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_octavesDecreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnExponentIncreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_exponentIncreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnExponentDecreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_exponentDecreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnNextNoiseTypeButtonClickCallback(const std::function<void()> &callback) const
{
    m_nextNoiseTypeButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnPreviousNoiseTypeButtonClickCallback(const std::function<void()> &callback) const
{
    m_prevNoiseTypeButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnHeightStepOnButtonClickCallback(const std::function<void()> &callback) const
{
    m_heightStepOnButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnHeightStepOffButtonClickCallback(const std::function<void()> &callback) const
{
    m_heightStepOffButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnStepsNbIncreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_stepsNbIncreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::initOnStepsNbDecreaseButtonClickCallback(const std::function<void()> &callback) const
{
    m_stepsNbDecreaseButton->initOnClickCallback(callback);
}

void TerrainGenerationMenu::setSeedInputText(const std::string &text) const
{
    m_seedInput->setText(text);
}

void TerrainGenerationMenu::setFrequencyValueText(const std::string &value) const
{
    m_frequencyValueText->setContent(value);
}

void TerrainGenerationMenu::setOctavesValueText(const std::string &value) const
{
    m_octavesValueText->setContent(value);
}

void TerrainGenerationMenu::setExponentValueText(const std::string &value) const
{
    m_exponentValueText->setContent(value);
}

std::string TerrainGenerationMenu::getSeedInputText() const
{
    return m_seedInput->getText();
}

void TerrainGenerationMenu::setTerrainGenerationMenuVisibility(const bool isVisible) const
{
    for (auto *widget : m_menuWidgets)
        widget->setVisibility(isVisible);
}

void TerrainGenerationMenu::selectNoiseType(const std::string &noiseTypeName) const
{
    m_noiseTypeValueText->setContent(noiseTypeName);
}

void TerrainGenerationMenu::setHeightStepMode(const bool isOn) const
{
    m_heightStepOnButton->setSelected(isOn);
    m_heightStepOffButton->setSelected(!isOn);
}

void TerrainGenerationMenu::setStepsNbValueText(const std::string &stepsNb) const
{
    m_stepsNbValueText->setContent(stepsNb);
}

void TerrainGenerationMenu::setVisibility(bool visible) const
{
    for (auto *widget: m_widgets) {
        widget->setVisibility(visible);
    }
}

void TerrainGenerationMenu::initTerrainGenerationMenu(const sf::Vector2u &windowSize)
{
    sf::Vector2f menuSize = sf::Vector2f(400, 480);
    sf::Vector2f menuPosition = sf::Vector2f(windowSize.x / 2.0f - menuSize.x / 2.0f, windowSize.y / 2.0f - menuSize.y / 2.0f);
    m_TerrainGenerationMenuBox = UIFactory::createBox(menuPosition, menuSize);
    UIFactory::applyDefaultBoxStyle(m_TerrainGenerationMenuBox);

    m_closeTerrainGenerationMenuButton = UIFactory::createSpriteButton("assets/textures/ui/close_512.png", 
        menuPosition + sf::Vector2f(menuSize.x - 50, 10), {15, 15}, "Close", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_closeTerrainGenerationMenuButton, HighlightTextAlign::Down);

    m_TerrainGenerationMenuTitle = UIFactory::createText(menuPosition + sf::Vector2f(110, 20), "Terrain Generation", 18);
    UIFactory::applyDefaultTextStyle(m_TerrainGenerationMenuTitle, UIFactory::TextVariant::Title);

    m_seedText = UIFactory::createText(menuPosition + sf::Vector2f(20, 70), "Seed: ", 15);
    UIFactory::applyDefaultTextStyle(m_seedText, UIFactory::TextVariant::Value);
    m_frequencyText = UIFactory::createText(menuPosition + sf::Vector2f(20, 120), "Frequency: ", 15);
    UIFactory::applyDefaultTextStyle(m_frequencyText, UIFactory::TextVariant::Value);
    m_octavesText = UIFactory::createText(menuPosition + sf::Vector2f(20, 170), "Octaves: ", 15);
    UIFactory::applyDefaultTextStyle(m_octavesText, UIFactory::TextVariant::Value);
    m_exponentText = UIFactory::createText(menuPosition + sf::Vector2f(20, 220), "Exponent: ", 15);
    UIFactory::applyDefaultTextStyle(m_exponentText, UIFactory::TextVariant::Value);
    m_noiseTypeText = UIFactory::createText(menuPosition + sf::Vector2f(20, 270), "Noise Type: ", 15);
    UIFactory::applyDefaultTextStyle(m_noiseTypeText, UIFactory::TextVariant::Value);
    m_heightStepText = UIFactory::createText(menuPosition + sf::Vector2f(20, 320), "Height Step:", 15);
    UIFactory::applyDefaultTextStyle(m_heightStepText, UIFactory::TextVariant::Value);
    m_stepsNbText = UIFactory::createText(menuPosition + sf::Vector2f(20, 370), "Steps Nb:", 15);
    UIFactory::applyDefaultTextStyle(m_stepsNbText, UIFactory::TextVariant::Value);

    m_generateButton = UIFactory::createTextButton(menuPosition + sf::Vector2f(240, 420), "  Generate  ", 17);
    UIFactory::applyDefaultTextButtonStyle(m_generateButton, UIFactory::TextVariant::Default);
    m_generateButton->initBackgroundStatesColor( sf::Color(235, 225, 250), sf::Color(248, 246, 252), 
        sf::Color(248, 246, 252), sf::Color(210, 190, 240), sf::Color(235, 225, 250));
    m_cancelButton = UIFactory::createTextButton(menuPosition + sf::Vector2f(85, 420), "   Cancel   ", 17);
    UIFactory::applyDefaultTextButtonStyle(m_cancelButton, UIFactory::TextVariant::Default);

    m_seedInput = UIFactory::createTextInput(menuPosition + sf::Vector2f(140, 60), sf::Vector2f(160, 40), "Enter seed (Ex: 1337)", 18, true);
    m_seedInput->initCharacterLimit(10);
    UIFactory::applyDefaultTextInputStyle(m_seedInput);
    m_randomSeedButton = UIFactory::createSpriteButton("assets/textures/ui/random_512.png", 
        menuPosition + sf::Vector2f(315, 60), {18, 18}, "Random", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_randomSeedButton, HighlightTextAlign::Down);

    m_frequencyValueText = UIFactory::createText(menuPosition + sf::Vector2f(210, 115), "1.0", 20);
    UIFactory::applyDefaultTextStyle(m_frequencyValueText, UIFactory::TextVariant::Default);
    m_frequencyIncreaseButton = UIFactory::createSpriteButton("assets/textures/ui/add_512.png", 
        menuPosition + sf::Vector2f(265, 110), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_frequencyIncreaseButton, HighlightTextAlign::Down);
    m_frequencyDecreaseButton = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png", 
        menuPosition + sf::Vector2f(140, 110), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_frequencyDecreaseButton, HighlightTextAlign::Down);
    m_frequencyDecreaseButton->setContinuousClick(true, 0.1f);
    m_frequencyIncreaseButton->setContinuousClick(true, 0.1f);

    m_octavesValueText = UIFactory::createText(menuPosition + sf::Vector2f(210, 165), "1", 20);
    UIFactory::applyDefaultTextStyle(m_octavesValueText, UIFactory::TextVariant::Default);
    m_octavesIncreaseButton = UIFactory::createSpriteButton("assets/textures/ui/add_512.png", 
        menuPosition + sf::Vector2f(265, 160), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_octavesIncreaseButton, HighlightTextAlign::Down);
    m_octavesDecreaseButton = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png", 
        menuPosition + sf::Vector2f(140, 160), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_octavesDecreaseButton, HighlightTextAlign::Down);
    m_octavesDecreaseButton->setContinuousClick(true);
    m_octavesIncreaseButton->setContinuousClick(true);

    m_exponentValueText = UIFactory::createText(menuPosition + sf::Vector2f(210, 215), "1.0", 20);
    UIFactory::applyDefaultTextStyle(m_exponentValueText, UIFactory::TextVariant::Default);
    m_exponentIncreaseButton = UIFactory::createSpriteButton("assets/textures/ui/add_512.png", 
        menuPosition + sf::Vector2f(265, 210), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_exponentIncreaseButton, HighlightTextAlign::Down);
    m_exponentDecreaseButton = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png", 
        menuPosition + sf::Vector2f(140, 210), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_exponentDecreaseButton, HighlightTextAlign::Down);
    m_exponentDecreaseButton->setContinuousClick(true);
    m_exponentIncreaseButton->setContinuousClick(true);

    m_noiseTypeValueText = UIFactory::createText(menuPosition + sf::Vector2f(195, 267.5), "OpenSimplex2s", 16);
    UIFactory::applyDefaultTextStyle(m_noiseTypeValueText, UIFactory::TextVariant::Default);
    m_nextNoiseTypeButton = UIFactory::createSpriteButton("assets/textures/ui/next_64.png", 
        menuPosition + sf::Vector2f(330, 260), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_nextNoiseTypeButton, HighlightTextAlign::Down);
    m_prevNoiseTypeButton = UIFactory::createSpriteButton("assets/textures/ui/prev_64.png", 
        menuPosition + sf::Vector2f(140, 260), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_prevNoiseTypeButton, HighlightTextAlign::Down);

    m_heightStepOnButton = UIFactory::createTextButton(menuPosition + sf::Vector2f(140, 310), " On ", 14);
    UIFactory::applyDefaultTextButtonStyle(m_heightStepOnButton);
    m_heightStepOffButton = UIFactory::createTextButton(menuPosition + sf::Vector2f(210, 310), " Off ", 14);
    UIFactory::applyDefaultTextButtonStyle(m_heightStepOffButton);

    m_stepsNbValueText = UIFactory::createText(menuPosition + sf::Vector2f(210, 365), "1", 20);
    UIFactory::applyDefaultTextStyle(m_stepsNbValueText, UIFactory::TextVariant::Default);
    m_stepsNbIncreaseButton = UIFactory::createSpriteButton("assets/textures/ui/add_512.png", 
        menuPosition + sf::Vector2f(265, 360), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_stepsNbIncreaseButton, HighlightTextAlign::Down);
    m_stepsNbDecreaseButton = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png", 
        menuPosition + sf::Vector2f(140, 360), {18, 18}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_stepsNbDecreaseButton, HighlightTextAlign::Down);
    m_stepsNbDecreaseButton->setContinuousClick(true);
    m_stepsNbIncreaseButton->setContinuousClick(true);

    m_menuWidgets.push_back(m_TerrainGenerationMenuBox);
    m_menuWidgets.push_back(m_closeTerrainGenerationMenuButton);
    m_menuWidgets.push_back(m_TerrainGenerationMenuTitle);
    m_menuWidgets.push_back(m_seedText);
    m_menuWidgets.push_back(m_frequencyText);
    m_menuWidgets.push_back(m_octavesText);
    m_menuWidgets.push_back(m_exponentText);
    m_menuWidgets.push_back(m_generateButton);
    m_menuWidgets.push_back(m_cancelButton);
    m_menuWidgets.push_back(m_seedInput);
    m_menuWidgets.push_back(m_randomSeedButton);
    m_menuWidgets.push_back(m_frequencyValueText);
    m_menuWidgets.push_back(m_frequencyIncreaseButton);
    m_menuWidgets.push_back(m_frequencyDecreaseButton);
    m_menuWidgets.push_back(m_octavesValueText);
    m_menuWidgets.push_back(m_octavesIncreaseButton);
    m_menuWidgets.push_back(m_octavesDecreaseButton);
    m_menuWidgets.push_back(m_exponentValueText);
    m_menuWidgets.push_back(m_exponentIncreaseButton);
    m_menuWidgets.push_back(m_exponentDecreaseButton);
    m_menuWidgets.push_back(m_noiseTypeText);
    m_menuWidgets.push_back(m_noiseTypeValueText);
    m_menuWidgets.push_back(m_nextNoiseTypeButton);
    m_menuWidgets.push_back(m_prevNoiseTypeButton);
    m_menuWidgets.push_back(m_heightStepText);
    m_menuWidgets.push_back(m_heightStepOnButton);
    m_menuWidgets.push_back(m_heightStepOffButton);
    m_menuWidgets.push_back(m_stepsNbText);
    m_menuWidgets.push_back(m_stepsNbValueText);
    m_menuWidgets.push_back(m_stepsNbIncreaseButton);
    m_menuWidgets.push_back(m_stepsNbDecreaseButton);
}

void TerrainGenerationMenu::initWidgetsList()
{
    m_widgets.push_back(m_menuBox);
    m_widgets.push_back(m_MenuTitle);
    m_widgets.push_back(m_TerrainGenerationMenuButton);
    for (auto *widget : m_menuWidgets)
        m_widgets.push_back(widget);
}
