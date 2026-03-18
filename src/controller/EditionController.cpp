//
// Created by fran on 17/03/2026.
//

#include "EditionController.hpp"

EditionController::EditionController()
    : m_heightStep(1)
    , m_currentSelectionMode(SelectionMode::TILE)
    , m_currentTextureId(1)
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
    if (sf::Mouse::isButtonPressed(m_paintMouseButton) && m_ongoingPaintCommand != nullptr && selectedTiles.size() > 0)
        m_ongoingPaintCommand->AddTiles(selectedTiles, model, view);
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
    }
    // paint ending
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == m_paintMouseButton
        && m_ongoingPaintCommand != nullptr) {
        if (!m_ongoingPaintCommand->isEmpty())
            m_commandHistory.addCommand(std::move(m_ongoingPaintCommand), model, view);
        m_ongoingPaintCommand = nullptr;
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
