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

    void initOnTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback);
    void initOnCloseTerrainGenerationMenuButtonClickCallback(const std::function<void()> &callback);
    void initOnGenerateButtonClickCallback(const std::function<void()> &callback);
    void initOnSeedInputValidatedCallback(const std::function<void(const std::string&)> &callback);
    void initOnRandomSeedButtonClickCallback(const std::function<void()> &callback);
    void initOnFrequencyIncreaseButtonClickCallback(const std::function<void()> &callback);
    void initOnFrequencyDecreaseButtonClickCallback(const std::function<void()> &callback);
    void initOnOctavesIncreaseButtonClickCallback(const std::function<void()> &callback);
    void initOnOctavesDecreaseButtonClickCallback(const std::function<void()> &callback);
    void initOnExponentIncreaseButtonClickCallback(const std::function<void()> &callback);
    void initOnExponentDecreaseButtonClickCallback(const std::function<void()> &callback);
    void initOnNextNoiseTypeButtonClickCallback(const std::function<void()> &callback);
    void initOnPreviousNoiseTypeButtonClickCallback(const std::function<void()> &callback);
    void initOnHeightStepOnButtonClickCallback(const std::function<void()> &callback);
    void initOnHeightStepOffButtonClickCallback(const std::function<void()> &callback);
    void initOnStepsNbIncreaseButtonClickCallback(const std::function<void()> &callback);
    void initOnStepsNbDecreaseButtonClickCallback(const std::function<void()> &callback);

    void setSeedInputText(const std::string &text);
    void setFrequencyValueText(const std::string &value);
    void setOctavesValueText(const std::string &value);
    void setExponentValueText(const std::string &value);
    std::string getSeedInputText() const;

    void setTerrainGenerationMenuVisibility(bool isVisible);
    void selectNoiseType(const std::string &noiseTypeName);
    void setHeightStepMode(bool isOn);
    void setStepsNbValueText(const std::string &stepsNb);
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