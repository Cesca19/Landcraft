//
// Created by fran on 14/04/2026.
//

#ifndef LANDCRAFT_EDITIONVIEW_HPP
#define LANDCRAFT_EDITIONVIEW_HPP

#include "../../ui/UIFactory.hpp"

class EditionMenu {
public:
    EditionMenu(sf::Vector2f globalUIPosition);
    ~EditionMenu();
    void setEditionToolButtonOnCLickCallback(int toolId, const std::function<void()> &callback) const;
    void setUndoButtonOnClickCallback(const std::function<void()> &callback) const;
    void setRedoButtonOnClickCallback(const std::function<void()> &callback) const;
    void selectEditionTool(int toolId) const;
    void unselectEditionTool(int toolId) const;
private:
    void applyUIStyle();
    void initWidgetsList();

    std::vector<SpriteButton*> m_editionToolsButtons;
    Text *m_toolsText;
    Box *m_editionToolsBox;
    SpriteButton *m_undoButton;
    SpriteButton *m_redoButton;
    std::vector<IWidget *> m_widgets;
    sf::Vector2f m_editionToolsBoxPosition;
    sf::Vector2f m_globalUIStartPosition;
    sf::Vector2f m_toolsMenuStartPosition;
};


#endif //LANDCRAFT_EDITIONVIEW_HPP