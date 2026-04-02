//
// Created by fran on 17/03/2026.
//

#include "EditionController.hpp"

EditionController::EditionController()
    : m_currentEditionTool(0)
{
    m_editionTools.emplace_back(std::make_unique<ElevationTool>());
    m_editionTools.emplace_back(std::make_unique<PaintTool>());
}

void EditionController::handleEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view, SelectionController &selectionController)
{
    handleEditionToolSwitchEvents(event);
    m_editionTools[m_currentEditionTool]->handleEvents(window, event, model, view, selectionController, m_commandHistory);
    handleUndoRedoEvents(window, event, model, view);
}

void EditionController::handleContinuousEvents(const sf::RenderWindow &window, WorldModel &model, WorldView &view, SelectionController &selectionController)
{
    m_editionTools[m_currentEditionTool]->handleContinuousEvents(window, model, view, selectionController, m_commandHistory);
}

bool EditionController::isEditing() const
{
    return m_editionTools[m_currentEditionTool]->isEditing();
}

bool EditionController::isSelectionLocked() const
{
    return m_editionTools[m_currentEditionTool]->isSelectionLocked();
}

SelectionMode EditionController::getSelectionMode() const
{
    return m_editionTools[m_currentEditionTool]->getRequiredSelectionMode();
}

void EditionController::handleUndoRedoEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view)
{
    const bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    if (isCtrlPressed && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
        m_commandHistory.undoCommand(model, view);
    if (isCtrlPressed && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Y)
        m_commandHistory.redoCommand(model, view);
}

void EditionController::handleEditionToolSwitchEvents(const sf::Event &event)
{
    // keyboard
    if (!isEditing() && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        m_currentEditionTool ++;
        if (m_currentEditionTool > m_editionTools.size() - 1)
            m_currentEditionTool = 0;
    }
}