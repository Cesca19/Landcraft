//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_EDITIONCONTROLLER_HPP
#define LANDCRAFT_EDITIONCONTROLLER_HPP

#include "SelectionController.hpp"
#include "../model/WorldModel.hpp"
#include "../view/WorldView.hpp"
#include "../commands/CommandHistory.hpp"
#include "../editionTools/EditionTools.hpp"

class EditionController {
public:
    EditionController();
    void handleEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void handleContinuousEvents(sf::RenderWindow& window, WorldModel& model, WorldView& view);
    void update(float deltaTime, sf::RenderWindow& window, WorldModel& model, const WorldView& view, bool isNavigating);
    void draw(sf::RenderWindow& window, const Camera& camera, bool isNavigating);
private:
    void handleUndoRedoEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void handleEditionToolSwitchEvents(const sf::Event& event);

    CommandHistory m_commandHistory;
    SelectionController m_selectionController;
    int m_currentEditionTool;
    std::vector<std::unique_ptr<IEditionTool>> m_editionTools;
};

#endif //LANDCRAFT_EDITIONCONTROLLER_HPP