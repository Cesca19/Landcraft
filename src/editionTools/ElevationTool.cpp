//
// Created by fran on 24/03/2026.
//

#include "ElevationTool.hpp"

ElevationTool::ElevationTool()
    : m_heightStep(1)
    , m_isSelectionLocked(false)
    , m_currentSelectionMode(SelectionMode::TILE_CORNER)
    , m_ongoingEditCornersHeightCommand(nullptr)
    , m_continuousElevationInterval(0.25f)
    , m_lastMouseScreenPosition(-1, -1)
    , m_lastMouseWorldPosition(-1, -1)
    , m_mouseMovementThreshold(2.5f)
{
}

bool ElevationTool::isSelectionLocked() const
{
    return m_isSelectionLocked;
}

SelectionMode ElevationTool::getRequiredSelectionMode() const
{
    return m_currentSelectionMode;
}

void ElevationTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view,
                                 SelectionController &selectionController, CommandHistory &history)
{
    handleSelectionEvents(event);
    handleHeightEditingEvents(event, model, view, selectionController, history);
}

void ElevationTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, SelectionController &selectionController,
                                           CommandHistory &history)
{
    handleKeyBoardHeightEditingEvents(window, model, view, selectionController, history);
}

void ElevationTool::handleSelectionEvents(const sf::Event &event)
{
    // keyboard
    // -> selection mode switching
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
        m_currentSelectionMode = (m_currentSelectionMode == SelectionMode::TILE)
                        ? SelectionMode::TILE_CORNER
                        : SelectionMode::TILE;
}

void ElevationTool::handleHeightEditingEvents(const sf::Event& event, WorldModel& model, WorldView& view,
                                 const SelectionController &selectionController, CommandHistory &history)
{
    // mouse
    // if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
    //     && event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
    //     if (m_ongoingEditCornersHeightCommand == nullptr) {
    //         // const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
    //         // if (selectedCorners.empty()) return;
    //         // m_ongoingEditCornersHeightCommand = std::make_unique<EditTilesCornersHeightCommand>(selectedCorners, m_heightStep * static_cast<int>(event.mouseWheelScroll.delta));
    //         // m_ongoingEditCornersHeightCommand->execute(model, view);
    //     } else
    //         m_ongoingEditCornersHeightCommand->addCorners(m_heightStep  * static_cast<int>(event.mouseWheelScroll.delta), model, view);
    // }
    // // register the last control used in the previous if and use it here and then reset
    // if (event.type == sf::Event::KeyReleased
    //     && (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::RControl)
    //     && m_ongoingEditCornersHeightCommand != nullptr) {
    //     // history.addCommand(std::move(m_ongoingEditCornersHeightCommand), model, view);
    //     // m_ongoingEditCornersHeightCommand = nullptr;
    // }
}

void ElevationTool::handleKeyBoardHeightEditingEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view,
    const SelectionController &selectionController, CommandHistory &history)
{
    const bool isRaising = sf::Keyboard::isKeyPressed(sf::Keyboard::Add) || sf::Keyboard::isKeyPressed(sf::Keyboard::P)
    || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
    const bool isLowering = sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) || sf::Keyboard::isKeyPressed(sf::Keyboard::M);

    if (isRaising || isLowering) {
        const float heightFactor = (isRaising ? 1.0f : -1.0f ) * m_heightStep;
        if (m_ongoingEditCornersHeightCommand == nullptr)
            startContinuousElevation(window, model, view, selectionController, heightFactor);
        else
            updateContinuousElevation(window, model, view, selectionController, heightFactor);
    } else {
        if (m_ongoingEditCornersHeightCommand != nullptr)
            stopContinuousElevation(model, view, history);
    }
}

void ElevationTool::startContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view,
    const SelectionController &selectionController, const float heightStep)
{
    const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
    if (selectedCorners.empty()) return;
    m_ongoingEditCornersHeightCommand = std::make_unique<EditTilesCornersHeightCommand>(/*selectedCorners, heightStep*/);
    m_ongoingEditCornersHeightCommand->addCorners(selectedCorners, heightStep, model, view);
    // m_ongoingEditCornersHeightCommand->execute(model, view);
    m_isSelectionLocked = true;
    m_continuousElevationClock.restart();
    m_lastMouseScreenPosition = sf::Mouse::getPosition(window);
    m_lastMouseWorldPosition = selectionController.getMouseWorldPosition();
}

void ElevationTool::updateContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view,
    const SelectionController &selectionController, const float heightStep)
{
    if (m_ongoingEditCornersHeightCommand == nullptr)
        return;
    const sf::Vector2i currentMouseScreenPosition = sf::Mouse::getPosition(window);
    bool hasMouseMoved = MathUtils::distanceBetweenPoints(static_cast<sf::Vector2f>(m_lastMouseScreenPosition),
                                static_cast<sf::Vector2f>(currentMouseScreenPosition)) > m_mouseMovementThreshold;

    if (hasMouseMoved) {
        if (!m_isSelectionLocked) {
            const sf::Vector2i currentMouseWorldPosition = selectionController.getMouseWorldPosition();
            const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
            std::set<TileCorner *> cornersToElevate(selectedCorners.begin(), selectedCorners.end());
            std::set<TileCorner *> bresenhamLineCorners = getTilesCornersFromBresenhamLine(m_lastMouseWorldPosition, currentMouseWorldPosition, model);

            cornersToElevate.insert(bresenhamLineCorners.begin(), bresenhamLineCorners.end());
            if (!cornersToElevate.empty())
                m_ongoingEditCornersHeightCommand->addCorners({cornersToElevate.begin(), cornersToElevate.end()}, heightStep, model, view);
            m_lastMouseWorldPosition = currentMouseWorldPosition;
            m_lastMouseScreenPosition = currentMouseScreenPosition;
        }
        m_isSelectionLocked = false;
    } else {
        m_isSelectionLocked = true;
        if (m_continuousElevationClock.getElapsedTime().asSeconds() >= m_continuousElevationInterval) {
            const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
            if (selectedCorners.empty()) return;
            m_ongoingEditCornersHeightCommand->addCorners(selectedCorners, heightStep, model, view);
            m_continuousElevationClock.restart();
        }
    }
}

void ElevationTool::stopContinuousElevation(WorldModel &model, WorldView &view, CommandHistory &history)
{
    history.addCommand(std::move(m_ongoingEditCornersHeightCommand), model, view);
    m_ongoingEditCornersHeightCommand = nullptr;
    m_isSelectionLocked = false;
    m_continuousElevationClock.restart();
    m_lastMouseScreenPosition = sf::Vector2i(-1, -1);
    model.onTileCornerHeightChanged();
}

std::set<TileCorner *> ElevationTool::getTilesCornersFromBresenhamLine(sf::Vector2i startPosition,
    sf::Vector2i endPosition, WorldModel &model)
{
    std::set<TileCorner *> cornersToElevate = {};
    if (m_currentSelectionMode == SelectionMode::TILE_CORNER) {
        const std::vector<std::vector<std::unique_ptr<TileCorner>>> &worldTilesCorners = model.getCorners();
        if (worldTilesCorners.empty() || worldTilesCorners[0].empty())
            return cornersToElevate;
        const std::vector<sf::Vector2i> lineTilesPositions =
                MathUtils::getBresenhamLine(startPosition, endPosition);
        for (const sf::Vector2i& pos : lineTilesPositions)
            if (pos.y >= 0 && pos.y < static_cast<int>(worldTilesCorners.size())
            && pos.x >= 0 && pos.x < static_cast<int>(worldTilesCorners[0].size())) {
                cornersToElevate.insert(worldTilesCorners[pos.y][pos.x].get());
            }
    } else {
        const std::vector<std::vector<Tile>> &worldTiles = model.getTiles();
        if (worldTiles.empty() || worldTiles[0].empty())
            return cornersToElevate;

        const std::vector<sf::Vector2i> lineTilesPositions =
                MathUtils::getBresenhamLine(startPosition, endPosition);
        for (const sf::Vector2i& pos : lineTilesPositions)
            if (pos.y >= 0 && pos.y < static_cast<int>(worldTiles.size())
            && pos.x >= 0 && pos.x < static_cast<int>(worldTiles[0].size())) {
                const std::vector<TileCorner *>  corners = worldTiles[pos.y][pos.x].getCorners();
                cornersToElevate.insert(corners.begin(), corners.end());
            }
    }
    return cornersToElevate;
}