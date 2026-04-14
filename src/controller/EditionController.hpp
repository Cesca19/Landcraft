//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_EDITIONCONTROLLER_HPP
#define LANDCRAFT_EDITIONCONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"
#include "../commands/CommandHistory.hpp"
#include "../editionTools/EditionTools.hpp"
#include "../view/menu/EditionMenu.hpp"

class EditionController {
public:
    EditionController(WorldModel &model, WorldView &view, sf::Vector2f globalUIPosition);
    void handleEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view, SelectionController& selectionController);
    void handleContinuousEvents(const sf::RenderWindow& window, WorldModel& model, WorldView& view, SelectionController& selectionController);
    bool isEditing() const;
    bool isSelectionLocked() const;
    SelectionMode getSelectionMode() const;
private:
    void handleUndoRedoEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void handleEditionToolSwitchEvents(const sf::Event& event);
    void selectEditionTool(int toolId);

    CommandHistory m_commandHistory;
    int m_currentEditionTool;
    std::vector<std::unique_ptr<IEditionTool>> m_editionTools;
    std::unique_ptr<EditionMenu> m_editionView;
    sf::Vector2f m_editionToolsBoxPosition;
    sf::Vector2f m_toolsMenuStartPosition;
};

#endif //LANDCRAFT_EDITIONCONTROLLER_HPP