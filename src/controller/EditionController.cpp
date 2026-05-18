//
// Created by fran on 17/03/2026.
//

#include <stdexcept>
#include "EditionController.hpp"

EditionController::EditionController(WorldModel &model, WorldView &view, const sf::Vector2f globalUIPosition)
    : m_currentEditionTool(-1)
    , m_editionView(std::make_unique<EditionMenu>(globalUIPosition))
    , m_editionToolsBoxPosition(0, 250)
    , m_toolsMenuStartPosition(m_editionToolsBoxPosition + sf::Vector2f(95, 0))
{
    m_editionTools.emplace_back(std::make_unique<ElevationTool>(m_toolsMenuStartPosition));
    m_editionTools.emplace_back(std::make_unique<PaintTool>(m_toolsMenuStartPosition));

    for (int i = 0; i < m_editionTools.size(); i++ ) {
        m_editionView->setEditionToolButtonOnCLickCallback(i, [this, i] () {
            this->selectEditionTool(i);
        });
    }
    m_editionView->setUndoButtonOnClickCallback([this, &model, &view] () {
        m_commandHistory.undoCommand(model, view);
    });
    m_editionView->setRedoButtonOnClickCallback([this, &model, &view] () {
        m_commandHistory.redoCommand(model, view);
    });
    selectEditionTool(0);
}

void EditionController::handleEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view, BrushController &brushController)
{
    handleEditionToolSwitchEvents(event);
    m_editionTools[m_currentEditionTool]->handleEvents(window, event, model, view, brushController, m_commandHistory);
    handleUndoRedoEvents(window, event, model, view);
}

void EditionController::handleContinuousEvents(const sf::RenderWindow &window, WorldModel &model, WorldView &view, BrushController &brushController)
{
    m_editionTools[m_currentEditionTool]->handleContinuousEvents(window, model, view, brushController, m_commandHistory);
}

bool EditionController::isEditing() const
{
    return m_editionTools[m_currentEditionTool]->isEditing();
}

bool EditionController::isSelectionLocked() const
{
    return m_editionTools[m_currentEditionTool]->isSelectionLocked();
}

bool EditionController::areEditableTilesVisible() const
{
    return m_editionTools[m_currentEditionTool]->areEditableTilesVisible();
}

SelectionMode EditionController::getSelectionMode() const
{
    return m_editionTools[m_currentEditionTool]->getRequiredSelectionMode();
}

void EditionController::clearCommandHistory()
{
    m_commandHistory.clearHistory();
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
    if (!isEditing() && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T) {
        int nextTool = m_currentEditionTool + 1;
        if (nextTool > m_editionTools.size() - 1)
            nextTool = 0;
        selectEditionTool(nextTool);
    }
}

void EditionController::selectEditionTool(const int toolId)
{
    if (toolId == m_currentEditionTool)
        return;
    if (toolId > m_editionTools.size() - 1 || toolId < 0)
        throw std::out_of_range("Invalid tool id");
    if (m_currentEditionTool != -1) {
        m_editionView->unselectEditionTool(m_currentEditionTool);
        m_editionTools[m_currentEditionTool]->onToolUnSelected();
    }
    m_currentEditionTool = toolId;
    m_editionView->selectEditionTool(m_currentEditionTool);
    m_editionTools[m_currentEditionTool]->onToolSelected();
}