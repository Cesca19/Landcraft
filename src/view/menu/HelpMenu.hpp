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
    void setMenuPosition(const sf::Vector2f &windowSize);
    void setColumnPosition(const sf::Vector2f &columnPosition, const sf::Vector2f &columnSize,
        HelpSectionHeader *header, const std::vector<HelpControlRow *> &rows) const;
    void initWidgetsList();
    void initControlRows();

    static constexpr float s_columnGap = 24.f;
    static constexpr float s_columnSideMargin = 36.f;
    static constexpr float s_sectionHeaderHeight = 40.f;
    static constexpr float s_rowVerticalGap = 8.f;

    bool m_isVisible;
    float m_cardOffset;
    sf::Vector2f m_windowSize;
    sf::Vector2f m_cardPosition;
    sf::Vector2f m_cardSize;
    Box *m_cardBox;
    SpriteButton *m_backButton;
    Text *m_title;
    Text *m_subtitle;
    Box *m_navigationBox;
    Box *m_editionBox;
    Box *m_actionsBox;
    HelpSectionHeader *m_navigationHeader;
    HelpSectionHeader *m_editionHeader;
    HelpSectionHeader *m_actionsHeader;
    std::vector<HelpControlRow *> m_navigationRows;
    std::vector<HelpControlRow *> m_editionRows;
    std::vector<HelpControlRow *> m_actionsRows;
    Box *m_tipBox;
    Image *m_tipIcon;
    Text *m_tipTitle;
    Text *m_tipText;
    Image *m_tipImage;
    std::vector<IWidget *> m_widgets;
};

#endif //LANDCRAFT_HELPMENU_HPP
