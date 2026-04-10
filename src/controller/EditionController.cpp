//
// Created by fran on 17/03/2026.
//

#include "EditionController.hpp"

EditionController::EditionController()
    : m_currentEditionTool(0)
{
    m_editionTools.emplace_back(std::make_unique<ElevationTool>());
    m_editionTools.emplace_back(std::make_unique<PaintTool>());

    Box *box = UIFactory::createBox(sf::Vector2f(0, 0), {1200, 75});
    box->initColors(sf::Color(sf::Color(205, 185, 220)), sf::Color(255, 255, 255));
    SpriteButton *undoButton = UIFactory::createSpriteButton("assets/textures/ui/undo_512.png", 
        sf::Vector2f(10, 10), {32, 32}, "Undo", 15);
        undoButton->initBackgroundStatesColor(sf::Color(185, 185, 185), sf::Color(250, 250, 250), sf::Color(250, 239, 250), sf::Color(246, 246, 246), sf::Color(225, 225, 225));
        undoButton->initOutlineStatesColors(sf::Color(183, 183, 183), sf::Color(153, 153, 153), sf::Color::Cyan, sf::Color(163, 163, 163), sf::Color::White);

    m_editionToolsBox = UIFactory::createBox(sf::Vector2f(0, 75), {75, 800});
    m_editionToolsBox->initColors(sf::Color(205, 185, 220), sf::Color(255, 255, 255));
    
    sf::Vector2f toolStartPos(10, 100);
    SpriteButton *elevationButton = UIFactory::createSpriteButton("assets/textures/ui/elevation_tool_512.png", toolStartPos, {32, 32}, "Elevation", 15);
    SpriteButton *paintButton = UIFactory::createSpriteButton("assets/textures/ui/paint_palette_512.png", toolStartPos + sf::Vector2f(0, 75), {32, 32}, "Paint", 15);

    m_editionToolsButtons.emplace_back(elevationButton);
    m_editionToolsButtons.emplace_back(paintButton);

    for (SpriteButton* button : m_editionToolsButtons) {
        button->initOutlineStatesColors(sf::Color(183, 183, 183), sf::Color(153, 153, 153), sf::Color::Cyan, sf::Color(163, 163, 163), sf::Color::White);
        button->initBackgroundStatesColor(sf::Color::Transparent /*sf::Color(185, 185, 185)*/, sf::Color(250, 250, 250), sf::Color(250, 239, 250), sf::Color(246, 246, 246), sf::Color(225, 225, 225));
        button->initHighlightTextAlign(HighlightTextAlign::Top);
    }

   m_editionToolsButtons[m_currentEditionTool]->setSelected(true);

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