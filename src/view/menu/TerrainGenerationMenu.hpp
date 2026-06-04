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

    void setTerrainGenerationMenuVisibility(bool isVisible);
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
    Text *m_ExponentText;
    TextButton *m_generateButton;
    TextButton *m_cancelButton;
    TextInput *m_seedInput;
    SpriteButton *m_randomSeedButton;

    std::vector<IWidget *> m_menuWidgets;
    std::vector<IWidget *> m_widgets;
};


#endif //LANDCRAFT_TERRAINGENERATIONMENU_HPP