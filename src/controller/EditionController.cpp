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

void EditionController::handleEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view)
{
    handleEditionToolSwitchEvents(event);
    m_editionTools[m_currentEditionTool]->handleEvents(window, event, model, view, m_selectionController, m_commandHistory);
    handleUndoRedoEvents(window, event, model, view);
}

void EditionController::handleContinuousEvents(sf::RenderWindow &window, WorldModel &model, WorldView &view)
{
    m_editionTools[m_currentEditionTool]->handleContinuousEvents(window, model, view, m_selectionController, m_commandHistory);
}

void EditionController::update(const float deltaTime, sf::RenderWindow &window, WorldModel &model, const WorldView &view, const bool isNavigating)
{
    bool hasModelChanged = false;
    if (!isNavigating && !m_editionTools[m_currentEditionTool]->isSelectionLocked())
        m_selectionController.update(deltaTime, window, m_editionTools[m_currentEditionTool]->getRequiredSelectionMode(),
            model, view.getCamera(), hasModelChanged);
}

void EditionController::draw(sf::RenderWindow &window, const Camera &camera, bool isNavigating)
{
    if (!isNavigating) // only draw selection when not panning or rotating to avoid visual clutter
        m_selectionController.draw(window, camera);
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
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        m_currentEditionTool ++;
        if (m_currentEditionTool > m_editionTools.size() - 1)
            m_currentEditionTool = 0;
    }
}