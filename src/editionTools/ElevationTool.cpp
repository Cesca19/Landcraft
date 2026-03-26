//
// Created by fran on 24/03/2026.
//

#include "ElevationTool.hpp"

ElevationTool::ElevationTool()
    : m_heightStep(0.1)
    , m_isSelectionLocked(false)
    , m_currentSelectionMode(SelectionMode::TILE_CORNER)
    , m_ongoingEditCornersHeightCommand(nullptr)
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

void ElevationTool::handleEvents(const sf::Event &event, WorldModel &model, WorldView &view,
                                 SelectionController &selectionController, CommandHistory &history)
{
    handleSelectionEvents(event);
    handleHeightEditingEvents(event, model, view, selectionController, history);
}

void ElevationTool::handleContinuousEvents(WorldModel &model, WorldView &view, SelectionController &selectionController,
                                           CommandHistory &history)
{
    handleKeyBoardHeightEditingEvents(model, view, selectionController, history);
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
    // unlock the selection on mouse move
    if (event.type == sf::Event::MouseMoved) {
        m_isSelectionLocked = false;
    }


    // // -> corners editing
    // if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add)
    //     updateSelectedCornersHeight(model, view, m_heightStep, selectionController, history);
    // if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract)
    //     updateSelectedCornersHeight(model, view, -m_heightStep, selectionController, history);


    // mouse
    // if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
    //     && event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
    //     if (m_ongoingEditCornersHeightCommand == nullptr) {
    //         // const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
    //         // if (selectedCorners.empty()) return;
    //         // m_ongoingEditCornersHeightCommand = std::make_unique<EditTilesCornersHeightCommand>(selectedCorners, m_heightStep * static_cast<int>(event.mouseWheelScroll.delta));
    //         // m_ongoingEditCornersHeightCommand->execute(model, view);
    //     } else
    //         m_ongoingEditCornersHeightCommand->addHeight(m_heightStep  * static_cast<int>(event.mouseWheelScroll.delta), model, view);
    // }
    // // register the last control used in the previous if and use it here and then reset
    // if (event.type == sf::Event::KeyReleased
    //     && (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::RControl)
    //     && m_ongoingEditCornersHeightCommand != nullptr) {
    //     // history.addCommand(std::move(m_ongoingEditCornersHeightCommand), model, view);
    //     // m_ongoingEditCornersHeightCommand = nullptr;
    // }
}

void ElevationTool::handleKeyBoardHeightEditingEvents(WorldModel &model, WorldView &view,
    const SelectionController &selectionController, CommandHistory &history)
{
    const bool isRaising = sf::Keyboard::isKeyPressed(sf::Keyboard::Add) || sf::Keyboard::isKeyPressed(sf::Keyboard::P);
    const bool isLowering = sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) || sf::Keyboard::isKeyPressed(sf::Keyboard::M);

    if (isRaising || isLowering) {
        if (m_ongoingEditCornersHeightCommand == nullptr) {
            const int heightFactor = (isRaising) ? 1 : -1;
            startContinuousElevation(model, view, selectionController, heightFactor * m_heightStep);
        } else {
            updateContinuousElevation(model, view, selectionController);
        }
    } else {
        if (m_ongoingEditCornersHeightCommand != nullptr)
            stopContinuousElevation(model, view, history);
    }
}

void ElevationTool::startContinuousElevation(WorldModel &model, WorldView &view,
    const SelectionController &selectionController, float heightStep)
{
    const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
    if (selectedCorners.empty()) return;
    m_ongoingEditCornersHeightCommand = std::make_unique<EditTilesCornersHeightCommand>(selectedCorners, heightStep);
    m_ongoingEditCornersHeightCommand->execute(model, view);
    m_isSelectionLocked = true;
}

void ElevationTool::updateContinuousElevation(WorldModel &model, const WorldView &view,
    const SelectionController &selectionController)
{
    if (m_ongoingEditCornersHeightCommand == nullptr)
        return;
    const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
    if (selectedCorners.empty()) return;
    m_ongoingEditCornersHeightCommand->addNewCorners(selectedCorners, model, view);
}

void ElevationTool::stopContinuousElevation(WorldModel &model, WorldView &view, CommandHistory &history)
{
    history.addCommand(std::move(m_ongoingEditCornersHeightCommand), model, view);
    m_ongoingEditCornersHeightCommand = nullptr;
    m_isSelectionLocked = false;
}
