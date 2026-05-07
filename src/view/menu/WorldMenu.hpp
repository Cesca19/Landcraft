//
// Created by fran on 07/05/2026.
//

#ifndef LANDCRAFT_WORLDMENU_HPP
#define LANDCRAFT_WORLDMENU_HPP

#include "../../ui/UIFactory.hpp"

class WorldMenu {
public:
    WorldMenu(sf::Vector2f globalUIPosition, const sf::Vector2f &quitMenuPosition);
    ~WorldMenu();
    void setSaveMapButtonOnClickCallback(const std::function<void()> &callback) const;
    void setDontSaveButtonOnClickCallback(const std::function<void()> &callback) const;
    void setCancelButtonOnClickCallback(const std::function<void()> &callback) const;
    void setQuitMenuVisibility(bool isVisible) const;
    void setQuitMenuPosition(const sf::Vector2f &position) const;
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
    TextButton *m_wireframeModeButton;
    TextButton *m_shadedModeButton;
    std::vector<IWidget *> m_widgets;
};


#endif //LANDCRAFT_WORLDMENU_HPP