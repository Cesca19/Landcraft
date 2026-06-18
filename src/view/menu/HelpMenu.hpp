//
// Created by fran on 17/06/2026.
//

#ifndef LANDCRAFT_HELPMENU_HPP
#define LANDCRAFT_HELPMENU_HPP

#include "../../ui/UIFactory.hpp"

class HelpMenu {
public:
    HelpMenu(const sf::Vector2f &windowSize);
    ~HelpMenu();
    void setVisibility(bool isVisible);
    bool isVisible() const;
    void onWindowResized(const sf::Vector2f &windowSize);
    void setCloseButtonOnClickCallback(const std::function<void()> &callback) const;
private:
    void setPosition(const sf::Vector2f &windowSize);
    void initWidgetsList();

    bool m_isVisible;
    sf::Vector2f m_windowSize;
    Box *m_menuBox;
    Text *m_title;
    Text *m_controlsText;
    TextButton *m_closeButton;
    std::vector<IWidget *> m_widgets;
};

#endif //LANDCRAFT_HELPMENU_HPP
