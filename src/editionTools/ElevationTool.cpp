//
// Created by fran on 24/03/2026.
//

#include "ElevationTool.hpp"

ElevationTool::ElevationTool()
    : m_heightStep(1)
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
    // -> corners editing
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add)
        updateSelectedCornersHeight(model, view, m_heightStep, selectionController, history);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract)
        updateSelectedCornersHeight(model, view, -m_heightStep, selectionController, history);


    // mouse
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
        && event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        if (m_ongoingEditCornersHeightCommand == nullptr) {
            const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
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
        history.addCommand(std::move(m_ongoingEditCornersHeightCommand), model, view);
        m_ongoingEditCornersHeightCommand = nullptr;
    }
}

void ElevationTool::updateSelectedCornersHeight(WorldModel &model, WorldView &view, int heightStep,
    const SelectionController &selectionController, CommandHistory &history)
{
    const std::vector<TileCorner *> selectedCorners = selectionController.getSelectedTileCorners();
    if (selectedCorners.empty()) return;
    history.addCommand(std::make_unique<EditTilesCornersHeightCommand>
        (selectedCorners, heightStep), model, view, true);
}
