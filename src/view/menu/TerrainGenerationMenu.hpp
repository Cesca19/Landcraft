//
// Created by fran on 02/06/2026.
//

#ifndef LANDCRAFT_TERRAINGENERATIONMENU_HPP
#define LANDCRAFT_TERRAINGENERATIONMENU_HPP

#include "../../ui/UIFactory.hpp"

class TerrainGenerationMenu {
public:
    TerrainGenerationMenu(const sf::Vector2f &terrainGenerationMenuPosition, const sf::Vector2u &windowSize);
    ~TerrainGenerationMenu();

    void initOnTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback) const;
    void initOnCloseTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback) const;
    void initOnGenerateButtonClickCallback(const std::function<void()> &callback) const;
    void initOnSeedInputValidatedCallback(const std::function<void(const std::string&)> &callback) const;
    void initOnRandomSeedButtonClickCallback(const std::function<void()> &callback) const;
    void initOnFrequencyIncreaseButtonClickCallback(const std::function<void()> &callback) const;
    void initOnFrequencyDecreaseButtonClickCallback(const std::function<void()> &callback) const;
    void initOnOctavesIncreaseButtonClickCallback(const std::function<void()> &callback) const;
    void initOnOctavesDecreaseButtonClickCallback(const std::function<void()> &callback) const;
    void initOnExponentIncreaseButtonClickCallback(const std::function<void()> &callback) const;
    void initOnExponentDecreaseButtonClickCallback(const std::function<void()> &callback) const;
    void initOnNextNoiseTypeButtonClickCallback(const std::function<void()> &callback) const;
    void initOnPreviousNoiseTypeButtonClickCallback(const std::function<void()> &callback) const;
    void initOnHeightStepOnButtonClickCallback(const std::function<void()> &callback) const;
    void initOnHeightStepOffButtonClickCallback(const std::function<void()> &callback) const;
    void initOnStepsNbIncreaseButtonClickCallback(const std::function<void()> &callback) const;
    void initOnStepsNbDecreaseButtonClickCallback(const std::function<void()> &callback) const;

    void setSeedInputText(const std::string &text) const;
    void setFrequencyValueText(const std::string &value) const;
    void setOctavesValueText(const std::string &value) const;
    void setExponentValueText(const std::string &value) const;
    std::string getSeedInputText() const;

    void setTerrainGenerationMenuVisibility(bool isVisible) const;
    void selectNoiseType(const std::string &noiseTypeName) const;
    void setHeightStepMode(bool isOn) const;
    void setStepsNbValueText(const std::string &stepsNb) const;
private:
    void initTerrainGenerationMenu(const sf::Vector2u &windowSize);
    void initWidgetsList();

    Box *m_menuBox;
    Text * m_MenuTitle;
    SpriteButton *m_TerrainGenerationMenuButton;
    
    Box *m_TerrainGenerationMenuBox;
    SpriteButton *m_closeTerrainGenerationMenuButton;
    Text *m_TerrainGenerationMenuTitle;
    Text *m_seedText;
    Text *m_frequencyText;
    Text *m_octavesText;
    Text *m_exponentText;
    TextButton *m_generateButton;
    TextButton *m_cancelButton;
    TextInput *m_seedInput;
    SpriteButton *m_randomSeedButton;
    Text *m_frequencyValueText;
    SpriteButton *m_frequencyIncreaseButton;
    SpriteButton *m_frequencyDecreaseButton;
    Text *m_octavesValueText;
    SpriteButton *m_octavesIncreaseButton;
    SpriteButton *m_octavesDecreaseButton;
    Text *m_exponentValueText;
    SpriteButton *m_exponentIncreaseButton;
    SpriteButton *m_exponentDecreaseButton;
    Text *m_noiseTypeText;
    Text *m_noiseTypeValueText;
    SpriteButton *m_nextNoiseTypeButton;
    SpriteButton *m_prevNoiseTypeButton;
    Text *m_heightStepText;
    TextButton *m_heightStepOnButton;
    TextButton *m_heightStepOffButton;
    Text *m_stepsNbText;
    Text *m_stepsNbValueText;
    SpriteButton *m_stepsNbIncreaseButton;
    SpriteButton *m_stepsNbDecreaseButton;

    std::vector<IWidget *> m_menuWidgets;
    std::vector<IWidget *> m_widgets;
};


#endif //LANDCRAFT_TERRAINGENERATIONMENU_HPP