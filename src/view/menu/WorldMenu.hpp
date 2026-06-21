//
// Created by fran on 07/05/2026.
//

#ifndef LANDCRAFT_WORLDMENU_HPP
#define LANDCRAFT_WORLDMENU_HPP

#include "../../ui/UIFactory.hpp"

enum class DrawMode {
    WIREFRAME,
    SHADED,
    WIREFRAME_SHADED
};

class WorldMenu {
public:
    WorldMenu(sf::Vector2f globalUIPosition,const sf::Vector2f &drawModesMenuPosition, const sf::Vector2f &mapNamePosition);
    ~WorldMenu();
    void initQuitMenu(const sf::Vector2f &quitMenuPosition);
    void initWaterLevelMenu(const sf::Vector2f &waterLevelMenuPosition);
    void setSaveMapButtonOnClickCallback(const std::function<void()> &callback) const;
    void setDontSaveButtonOnClickCallback(const std::function<void()> &callback) const;
    void setCancelButtonOnClickCallback(const std::function<void()> &callback) const;
    void setQuitMenuVisibility(bool isVisible);
    bool isQuitMenuVisible() const;
    void setQuitMenuPosition(const sf::Vector2f &position) const;
    void setDrawModeButtonOnClickCallback(DrawMode mode, const std::function<void()> &callback);
    void selectDrawModeButton(DrawMode mode);
    void unselectDrawModeButton(DrawMode mode);
    void setMapName(const std::string &mapName);
    void initOnWaterLevelIncrementButtonClickCallback(const std::function<void()> &callback) const;
    void initOnWaterLevelDecrementButtonClickCallback(const std::function<void()> &callback) const;
    void setWaterLevelValueText(const std::string &value) const;
    void setVisibility(bool visible) const;
private:
    void updateMapNameMenu();
    void initWidgetsList();

    sf::Vector2f m_mapNameMenuPosition;
    Box *m_globalMenuBox;
    Box *m_quitMenuBox;
    Text *m_quitMenuTitle;
    Text *m_quitMenuDescription;
    TextButton *m_saveMapButton;
    TextButton *m_dontSaveButton;
    TextButton *m_cancelButton;
    Box *m_wireframeShadedModeBox;
    SpriteButton *m_shadedModeButton;
    SpriteButton *m_wireframeModeButton;
    SpriteButton *m_wireframeShadedModeButton;
    Text *m_drawModeTitle;
    TextButton *m_mapNameBtn;
    bool m_isQuitMenuVisible;
    Box *m_waterLevelMenuBox;
    Text *m_waterLevelTitle;
    Text *m_waterLevelValueText;
    SpriteButton *m_decrementWaterLevel;
    SpriteButton *m_incrementWaterLevel;
    std::vector<IWidget *> m_widgets;
    std::unordered_map<DrawMode, SpriteButton*> m_drawModeButtons;
};


#endif //LANDCRAFT_WORLDMENU_HPP