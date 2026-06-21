//
// Created by fran on 06/05/2026.
//

#ifndef LANDCRAFT_MAPSAVELOADMENU_HPP
#define LANDCRAFT_MAPSAVELOADMENU_HPP

#include "../../ui/UIFactory.hpp"

class MapSaveLoadMenu {
public:
    MapSaveLoadMenu(sf::Vector2f globalUIPosition);
    ~MapSaveLoadMenu();
    void setSaveFileButtonOnClickCallback(const std::function<void()> &callback) const;
    void setLoadFileButtonOnClickCallback(const std::function<void()> &callback) const;
    void setVisibility(bool visible) const;
private:
    void initWidgetsList();

    Box *m_menuBox;
    Text *m_menuTitle;
    SpriteButton *m_saveFileButton;
    SpriteButton *m_loadFileButton;
    std::vector<IWidget *> m_widgets;
    sf::Vector2f m_globalUIStartPosition;
};


#endif //LANDCRAFT_MAPSAVELOADMENU_HPP