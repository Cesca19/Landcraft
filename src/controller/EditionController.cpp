//
// Created by fran on 17/03/2026.
//

#include "EditionController.hpp"

EditionController::EditionController()
    : m_heightStep(1)
    , m_currentSelectionMode(SelectionMode::TILE)
    , m_currentTextureId(1)
    , m_lastPaintedTile(nullptr)
{
}

void EditionController::handleEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view)
{
    handleSelectionEvents(window, event, model, view);
    handleTilePaintingEvents(window, event, model, view);
    handleHeightEditingEvents(window, event, model, view);
    handleUndoRedoEvents(window, event, model, view);
}

void EditionController::handleContinuousEvents(sf::RenderWindow &window, WorldModel &model, WorldView &view)
{
    // tiles painting
    const std::vector<Tile *> selectedTiles = m_selectionController.getSelectedTiles();
    if (!sf::Mouse::isButtonPressed(m_paintMouseButton)
        || m_ongoingPaintCommand == nullptr
        || selectedTiles.empty())
        return;

    Tile *currentTile = selectedTiles[0];
    if (currentTile == m_lastPaintedTile)
        return;

    if (m_lastPaintedTile == nullptr) {
        m_ongoingPaintCommand->AddTiles(selectedTiles, model, view);
        m_lastPaintedTile = m_ongoingPaintCommand->getLastPaintedTile();
        return;
	}

    std::vector<std::vector<Tile>> &worldTiles = model.getTiles();
    if (worldTiles.empty() || worldTiles[0].empty())
        return;

    std::vector<sf::Vector2i> lineTilesPositions = 
            getBresenhamLine(m_lastPaintedTile->getGridPosition(), currentTile->getGridPosition());
    for (const sf::Vector2i& pos : lineTilesPositions)
        if (pos.y >= 0 && pos.y < static_cast<int>(worldTiles.size()) 
        && pos.x >= 0 && pos.x < static_cast<int>(worldTiles[0].size())) {
            m_ongoingPaintCommand->AddTile(&worldTiles[pos.y][pos.x], model, view);
        }
    m_lastPaintedTile = m_ongoingPaintCommand->getLastPaintedTile();
}

void EditionController::update(float deltaTime, sf::RenderWindow &window, WorldModel &model, WorldView &view, bool isNavigating)
{
    bool hasModelChanged = false;
    if (!isNavigating)
        m_selectionController.update(deltaTime, window, m_currentSelectionMode,
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

void EditionController::handleSelectionEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view)
{
    // keyboard
    // -> selection mode switching
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
        m_currentSelectionMode = (m_currentSelectionMode == SelectionMode::TILE)
                        ? SelectionMode::TILE_CORNER
                        : SelectionMode::TILE;
}

void EditionController::handleTilePaintingEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view)
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
        m_ongoingPaintCommand = std::make_unique<PaintTilesCommand>(m_selectionController.getSelectedTiles(), m_currentTextureId);
        m_ongoingPaintCommand->execute(model, view);
        m_lastPaintedTile = m_ongoingPaintCommand->getLastPaintedTile();
    }
    // paint ending
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == m_paintMouseButton
        && m_ongoingPaintCommand != nullptr) {
        if (!m_ongoingPaintCommand->isEmpty())
            m_commandHistory.addCommand(std::move(m_ongoingPaintCommand), model, view);
        m_ongoingPaintCommand = nullptr;
        m_lastPaintedTile = nullptr;
    }
}

void EditionController::handleHeightEditingEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view)
{
    // -> corners editing
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add)
        updateSelectedCornersHeight(model, view, m_heightStep);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract)
        updateSelectedCornersHeight(model, view, -m_heightStep);

    // mouse
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
        && event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
        updateSelectedCornersHeight(model, view, m_heightStep * static_cast<int>(event.mouseWheelScroll.delta));
}

void EditionController::updateSelectedCornersHeight(WorldModel &model, WorldView &view, int heightStep)
{
    const std::vector<TileCorner *> selectedCorners = m_selectionController.getSelectedTileCorners();
    if (selectedCorners.size() == 0) return;
    m_commandHistory.addCommand(std::make_unique<SetTilesCornersHeightCommand>
        (selectedCorners, heightStep), model, view);
}

std::vector<sf::Vector2i> EditionController::getBresenhamLine(sf::Vector2i start, sf::Vector2i end) const
{
    std::vector<sf::Vector2i> linePoints;
    int dx = std::abs(end.x - start.x);
    int dy = std::abs(end.y - start.y);

    int mainAxisStep = (dx > dy) ? dx : dy;
    int secondaryAxisStep = (dx > dy) ? dy : dx;

    int stepX = (end.x > start.x) ? 1 : -1;
    int stepY = (end.y > start.y) ? 1 : -1;

    int slopeError = 0;
    int x = start.x;
    int y = start.y;

    linePoints.push_back(sf::Vector2i(x, y));
    while (x != end.x || y != end.y) {
        if (dx > dy)
            x += stepX;
        else
            y += stepY;
        // mathematically we should have slopeError += (secondaryAxisStep / mainAxisStep) 
        // but to avoid floating point precision issues by multiplying everything by mainAxisStep * 2
        // slopeError += (secondaryAxisStep / mainAxisStep) * 2 * mainAxisStep;
        // we finally end up with 
        slopeError += secondaryAxisStep * 2;
        // we do the same thing for the comparison with 0.5, 
        // we multiply by mainAxisStep * 2 to avoid floating point precision issues
        // so instead of comparing slopeError >= 0.5 we compare 
        // slopeError >= (0.5 * mainAxisStep * 2) an finally we end up with
        if (slopeError >= mainAxisStep) {
            if (dx > dy)
                y += stepY;
            else
                x += stepX;
            // and we should reset the slope error by subtracting 1 but since the other 
            // operations are multiplied by mainAxisStep * 2 we need to do the same 
            // for the reset, so we subtract mainAxisStep * 2 instead of just 1
            slopeError -= mainAxisStep * 2;
        }
        linePoints.push_back(sf::Vector2i(x, y));
    }
    return linePoints;
}
