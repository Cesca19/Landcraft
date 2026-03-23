//
// Created by fran on 17/03/2026.
//

#include "EditionController.hpp"

EditionController::EditionController()
    : m_heightStep(1)
    , m_currentTextureId(1)
    , m_currentSelectionMode(SelectionMode::TILE)
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

    const Tile *currentTile = selectedTiles[0];
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

    const std::vector<sf::Vector2i> lineTilesPositions =
            MathUtils::getBresenhamLine(m_lastPaintedTile->getGridPosition(), currentTile->getGridPosition());
    for (const sf::Vector2i& pos : lineTilesPositions)
        if (pos.y >= 0 && pos.y < static_cast<int>(worldTiles.size()) 
        && pos.x >= 0 && pos.x < static_cast<int>(worldTiles[0].size())) {
            m_ongoingPaintCommand->AddTile(&worldTiles[pos.y][pos.x], model, view);
        }
    m_lastPaintedTile = m_ongoingPaintCommand->getLastPaintedTile();
}

void EditionController::update(const float deltaTime, sf::RenderWindow &window, WorldModel &model, const WorldView &view, const bool isNavigating)
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
        && event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        if (m_ongoingEditCornersHeightCommand == nullptr) {
            const std::vector<TileCorner *> selectedCorners = m_selectionController.getSelectedTileCorners();
            if (selectedCorners.empty()) return;
            m_ongoingEditCornersHeightCommand = std::make_unique<EditTilesCornersHeightCommand>(selectedCorners, m_heightStep * static_cast<int>(event.mouseWheelScroll.delta));
            m_ongoingEditCornersHeightCommand->execute(model, view);
        } else
            m_ongoingEditCornersHeightCommand->addHeight(m_heightStep  * static_cast<int>(event.mouseWheelScroll.delta), model, view);
    }
    // register the last control used in the previous if and use it here and then reset
    if (event.type == sf::Event::KeyReleased
        && (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::RControl)
        && m_ongoingEditCornersHeightCommand != nullptr) {
        m_commandHistory.addCommand(std::move(m_ongoingEditCornersHeightCommand), model, view);
        m_ongoingEditCornersHeightCommand = nullptr;
    }
}

void EditionController::updateSelectedCornersHeight(WorldModel &model, WorldView &view, int heightStep)
{
    const std::vector<TileCorner *> selectedCorners = m_selectionController.getSelectedTileCorners();
    if (selectedCorners.empty()) return;
    m_commandHistory.addCommand(std::make_unique<EditTilesCornersHeightCommand>
        (selectedCorners, heightStep), model, view, true);
}