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
    WorldMenu(sf::Vector2f globalUIPosition, const sf::Vector2f &quitMenuPosition);
    ~WorldMenu();
    void setSaveMapButtonOnClickCallback(const std::function<void()> &callback) const;
    void setDontSaveButtonOnClickCallback(const std::function<void()> &callback) const;
    void setCancelButtonOnClickCallback(const std::function<void()> &callback) const;
    void setQuitMenuVisibility(bool isVisible);
    bool isQuitMenuVisible() const;
    void setQuitMenuPosition(const sf::Vector2f &position) const;
    void setDrawModeButtonOnClickCallback(DrawMode mode, const std::function<void()> &callback);
    void selectDrawModeButton(DrawMode mode);
    void unselectDrawModeButton(DrawMode mode);
private:
    void initWidgetsList();

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
    bool m_isQuitMenuVisible;
    std::vector<IWidget *> m_widgets;
    std::unordered_map<DrawMode, SpriteButton*> m_drawModeButtons;
};


#endif //LANDCRAFT_WORLDMENU_HPP