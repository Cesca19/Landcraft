//
// Created by fran on 24/03/2026.
//

#include "PaintTool.hpp"

PaintTool::PaintTool()
    : m_isEditing(false)
    , m_currentTextureId(1)
    , m_previousMousePosition(-1, -1)
    , m_ongoingPaintCommand(nullptr)
{
}

bool PaintTool::isEditing() const
{
    return m_isEditing;
}

bool PaintTool::isSelectionLocked() const
{
    return false;
}

SelectionMode PaintTool::getRequiredSelectionMode() const
{
    return SelectionMode::TILE;
}

void PaintTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, SelectionController &selectionController,
                             CommandHistory &history)
{
    // tile painting texture picking
    // Later it will we be ui button that will change the current textureId
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Num0 || event.key.code == sf::Keyboard::Numpad0) m_currentTextureId = 0; // clear
        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) m_currentTextureId = 1; // grass
        if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) m_currentTextureId = 2; // water
        if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) m_currentTextureId = 3; // sand
    }

    // paint starting
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == m_paintMouseButton
        && m_ongoingPaintCommand == nullptr) {
        m_ongoingPaintCommand = std::make_unique<PaintTilesCommand>(selectionController.getSelectedTiles(), m_currentTextureId);
        m_ongoingPaintCommand->execute(model, view);
        m_previousMousePosition = selectionController.getMouseWorldPosition();
        m_isEditing = true;
    }
    // paint ending
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == m_paintMouseButton
        && m_ongoingPaintCommand != nullptr) {
        if (!m_ongoingPaintCommand->isEmpty())
            history.addCommand(std::move(m_ongoingPaintCommand), model, view);
        m_ongoingPaintCommand = nullptr;
        m_previousMousePosition = {-1, -1};
        m_isEditing = false;
    }
}

void PaintTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, SelectionController &selectionController,
    CommandHistory &history)
{
    // tiles painting
    const std::vector<Tile *>& selectedTiles = selectionController.getSelectedTiles();
    if (!sf::Mouse::isButtonPressed(m_paintMouseButton)
        || m_ongoingPaintCommand == nullptr
        || selectedTiles.empty())
        return;

    const sf::Vector2i currentMousePosition = selectionController.getMouseWorldPosition();
    if (currentMousePosition == m_previousMousePosition)
        return;

    if (m_previousMousePosition == sf::Vector2i{-1, -1}) {
        m_ongoingPaintCommand->AddTiles(selectedTiles, model, view);
        m_previousMousePosition = currentMousePosition;
        return;
    }

    std::vector<std::vector<Tile>> &worldTiles = model.getTiles();
    if (worldTiles.empty() || worldTiles[0].empty())
        return;

    const std::vector<sf::Vector2i> lineTilesPositions =
            MathUtils::getBresenhamLine(m_previousMousePosition, currentMousePosition);
    for (const sf::Vector2i& pos : lineTilesPositions)
        if (pos.y >= 0 && pos.y < static_cast<int>(worldTiles.size())
        && pos.x >= 0 && pos.x < static_cast<int>(worldTiles[0].size())) {
            m_ongoingPaintCommand->AddTile(&worldTiles[pos.y][pos.x], model, view);
        }
    m_previousMousePosition = currentMousePosition;
}
