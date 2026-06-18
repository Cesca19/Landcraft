//
// Created by fran on 17/06/2026.
//

#ifndef LANDCRAFT_STARTMENU_HPP
#define LANDCRAFT_STARTMENU_HPP

#include "../../ui/UIFactory.hpp"

class StartMenu {
public:
    StartMenu(const sf::Vector2f &windowSize);
    ~StartMenu();
    void setVisibility(bool isVisible);
    bool isVisible() const;
    void onWindowResized(const sf::Vector2f &windowSize);
    void setNewProjectButtonOnClickCallback(const std::function<void()> &callback) const;
    void setLoadMapButtonOnClickCallback(const std::function<void()> &callback) const;
    void setHelpButtonOnClickCallback(const std::function<void()> &callback) const;
private:
    void setMenuPosition(const sf::Vector2f &windowSize);
    void initWidgetsList();

    bool m_isVisible;
    float m_actionButtonHeight;
    float m_actionButtonWidth;
    float m_cardOffset;
    sf::Vector2f m_windowSize;
    sf::Vector2f m_cardPosition;
    sf::Vector2f m_cardSize;
    sf::Vector2f m_previewCenter;
    float m_previewScale;
    Box *m_cardBox;
    Text *m_brandTitle;
    Text *m_projectDescription;
    Image *m_previewImage;
    MenuActionButton *m_newProjectButton;
    MenuActionButton *m_loadMapButton;
    MenuActionButton *m_helpButton;
    std::vector<IWidget *> m_widgets;
};

#endif //LANDCRAFT_STARTMENU_HPP
