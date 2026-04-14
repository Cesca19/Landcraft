//
// Created by fran on 14/04/2026.
//

#ifndef LANDCRAFT_EDITIONVIEW_HPP
#define LANDCRAFT_EDITIONVIEW_HPP

#include "../ui/UIFactory.hpp"

class EditionView {
public:
    EditionView(sf::Vector2f globalUIPosition);
    ~EditionView();
    void setEditionToolButtonOnCLickCallback(int toolId, std::function<void()> callback);
    void setUndoButtonOnClickCallback(std::function<void()> callback);
    void setRedoButtonOnClickCallback(std::function<void()> callback);
    void selectEditionTool(int toolId);
    void unselectEditionTool(int toolId);
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