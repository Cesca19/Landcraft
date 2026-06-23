//
// Created by fran on 22/06/2026.
//

#include "SetHeightTool.hpp"

SetHeightTool::SetHeightTool(sf::Vector2f startMenuPosition)
    : m_isEditing(false)
    , m_isSelectionLocked(false)
    , m_currentSelectionMode(-1)
    , m_heightValue(5)
    , m_lastMouseScreenPosition(-1, -1)
    , m_lastMouseWorldPosition(-1, -1)
    , m_mouseMovementThreshold(2.5f)
    , m_ongoingSetTilesCornersHeightCommand(nullptr)
{
    m_selectionModes.push_back(SelectionMode::TILE_CORNER);
    m_selectionModes.push_back(SelectionMode::TILE);
    initMenuUI(startMenuPosition);
    initOnSetHeightValidateCallback([this] (const std::string& newValue) {
        this->updateHeightValue(newValue);
    });
    setUIVisibility(false);
    setSelectionMode(0);
}

SetHeightTool::~SetHeightTool()
{
}

bool SetHeightTool::isEditing() const
{
    return m_isEditing;
}

bool SetHeightTool::isSelectionLocked() const
{
    return m_isSelectionLocked;
}

SelectionMode SetHeightTool::getRequiredSelectionMode() const 
{
    return m_selectionModes[m_currentSelectionMode];
}

void SetHeightTool::onToolSelected() const
{
    setUIVisibility(true);
}

void SetHeightTool::onToolUnSelected() const
{
    setUIVisibility(false);
}

bool SetHeightTool::areEditableTilesVisible() const
{
    return true;
}

void SetHeightTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view,
                                 BrushController &brushController, CommandHistory &history)
{
    // keyboard
    // -> selection mode switching
    if (!m_isEditing && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        int nextMode = m_currentSelectionMode + 1;
        if (nextMode > m_selectionModes.size() - 1)
            nextMode = 0;
        setSelectionMode(nextMode);
    }
}

void SetHeightTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController,
                                           CommandHistory &history)
{
    bool isMouseButtonPressed = sf::Mouse::isButtonPressed(m_editingMouseButton);

    if (isMouseButtonPressed) {
        if (m_ongoingSetTilesCornersHeightCommand == nullptr)
            startContinuousElevation(window, model, view, brushController);
        else
            updateContinuousElevation(window, model, view, brushController);
        m_isEditing = true;
    } else {
        if (m_ongoingSetTilesCornersHeightCommand != nullptr)
            stopContinuousElevation(model, view, history);
        m_isEditing = false;
    }
}

void SetHeightTool::setVisibility(bool isVisible) const
{
    setUIVisibility(isVisible);
}

void SetHeightTool::initOnSetHeightValidateCallback(std::function<void(const std::string&)> callback)
{
    m_setHeightInput->initOnValidateCallback(callback);
}

std::string SetHeightTool::getSetHeightInputValue() const
{
    return m_setHeightInput->getText();
}

void SetHeightTool::startContinuousElevation(const sf::RenderWindow &window, WorldModel &model, const WorldView &view, const BrushController &brushController)
{
    const std::vector<BrushTileCornerHit> &brushSelection = (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER) ? brushController.getBrushTileCornersSelection() : brushController.getBrushTilesSelectionAsTileCorners();
    if (brushSelection.empty()) 
        return;
    updateHeightValue(getSetHeightInputValue());
    m_ongoingSetTilesCornersHeightCommand = std::make_unique<SetTilesCornersHeightCommand>(m_heightValue);
    m_ongoingSetTilesCornersHeightCommand->addCorners(brushSelection, model, view);
    m_isSelectionLocked = true;
    m_continuousElevationClock.restart();
    m_lastMouseScreenPosition = sf::Mouse::getPosition(window);
    m_lastMouseWorldPosition = brushController.getMouseWorldPosition();
}

void SetHeightTool::updateContinuousElevation(const sf::RenderWindow &window, WorldModel &model, const WorldView &view, const BrushController &brushController)
{
    if (m_ongoingSetTilesCornersHeightCommand == nullptr)
        return;
    const sf::Vector2i currentMouseScreenPosition = sf::Mouse::getPosition(window);
    const bool hasScreenMouseMoved = MathUtils::distanceBetweenPoints(static_cast<sf::Vector2f>(m_lastMouseScreenPosition),
                                static_cast<sf::Vector2f>(currentMouseScreenPosition)) > m_mouseMovementThreshold;
    const sf::Vector2i currentMouseWorldPosition = brushController.getMouseWorldPosition();
    const bool hasAnyMouseMoved = hasScreenMouseMoved || view.isMoving();

    if (!hasAnyMouseMoved) {
        m_isSelectionLocked = true;
        return;
    }
    // here the mouse is moving
    if (m_isSelectionLocked) { // wait for the next frame to let selection controller upd
        m_isSelectionLocked = false;
        return;
    }
    if (m_lastMouseWorldPosition != currentMouseWorldPosition) {
        applySetHeightAlongPath(currentMouseWorldPosition, model, view, brushController);
        m_lastMouseWorldPosition = currentMouseWorldPosition;
        m_lastMouseScreenPosition = currentMouseScreenPosition;
        m_continuousElevationClock.restart();
        m_isSelectionLocked = true;
    } else
        m_isSelectionLocked = false;
}

void SetHeightTool::stopContinuousElevation(WorldModel &model, WorldView &view, CommandHistory &history)
{
    if (m_ongoingSetTilesCornersHeightCommand == nullptr)
        return;
    history.addCommand(std::move(m_ongoingSetTilesCornersHeightCommand), model, view);
    m_ongoingSetTilesCornersHeightCommand = nullptr;
    m_isSelectionLocked = false;
    m_continuousElevationClock.restart();
    m_lastMouseScreenPosition = sf::Vector2i(-1, -1);
}

void SetHeightTool::applySetHeightOnCurrentSelection(WorldModel &model, const WorldView &view, const BrushController &brushController)
{
    const std::vector<BrushTileCornerHit> &brushSelection =  (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER) ? brushController.getBrushTileCornersSelection() : brushController.getBrushTilesSelectionAsTileCorners();
    if (brushSelection.empty()) return;
    m_ongoingSetTilesCornersHeightCommand->addCorners(brushSelection, model, view);
}

void SetHeightTool::applySetHeightAlongPath(sf::Vector2i targetPosition, WorldModel &model, const WorldView &view, const BrushController &brushController)
{
    const std::vector<sf::Vector2i> lineTilesPositions = MathUtils::getBresenhamLine(m_lastMouseWorldPosition, targetPosition);

    for (size_t i = 1; i < lineTilesPositions.size(); ++i) {
        const sf::Vector2i& pos = lineTilesPositions[i];
        std::vector<BrushTileCornerHit> stepSelection;
        if (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER)
            stepSelection = brushController.getNeighborsTileCornersInBrush(model, pos.x, pos.y);
        else
            stepSelection = brushController.getNeighborsTilesInBrushAsTileCorners(model, pos.x, pos.y);
        if (!stepSelection.empty())
            m_ongoingSetTilesCornersHeightCommand->addCorners(stepSelection, model, view);
    }
}

void SetHeightTool::updateHeightValue(std::string newValue)
{
    if (newValue.empty()) {
        return;
    }
    m_heightValue = std::stof(newValue);
}

void SetHeightTool::setSelectionMode(int index)
{
    if (index < 0 || index >= static_cast<int>(m_selectionModes.size()) || index == m_currentSelectionMode)
        return;
    if (m_currentSelectionMode != -1)
         m_selectionModesButtons[m_currentSelectionMode]->setSelected(false);
    m_currentSelectionMode = index;
    m_selectionModesButtons[m_currentSelectionMode]->setSelected(true);
}

void SetHeightTool::setUIVisibility(bool isVisible) const
{
    for (const auto widget : m_widgets)
        widget->setVisibility(isVisible);
}

void SetHeightTool::initMenuUI(sf::Vector2f startMenuPosition)
{
    m_setHeightToolBox = UIFactory::createBox(startMenuPosition, {190, 310});
    UIFactory::applyDefaultBoxStyle(m_setHeightToolBox);

    m_setHeightToolText = UIFactory::createText(startMenuPosition + sf::Vector2f(35, 10),"Set Height", 20);
    UIFactory::applyDefaultTextStyle(m_setHeightToolText, UIFactory::TextVariant::Title);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(35, 100);
    initSelectionModeUI(startMenuPosition + sf::Vector2f(0, 15));
    initSetHeightUI(startMenuPosition + sf::Vector2f(0, 35));
    initWidgetsList();
}

void SetHeightTool::initSelectionModeUI(sf::Vector2f startMenuPosition)
{
    m_selectionModeBox = UIFactory::createBox(startMenuPosition + sf::Vector2f(20, 45), {150, 130});
    UIFactory::applyDefaultBoxStyle(m_selectionModeBox);

    m_selectionModeText =  UIFactory::createText(startMenuPosition + sf::Vector2f(35, 55),"Selection Mode", 15);
    UIFactory::applyDefaultTextStyle(m_selectionModeText, UIFactory::TextVariant::Label);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(35, 100);
    SpriteButton *tileCornerMode = UIFactory::createSpriteButton("assets/textures/ui/corner_512.png", startBtnPosition + sf::Vector2f(0, 0),
        sf::Vector2f(32, 32), "Corner", 15);
    SpriteButton *tileMode = UIFactory::createSpriteButton("assets/textures/ui/tiles_512.png", startBtnPosition + sf::Vector2f(65, 0),
        sf::Vector2f(32, 32), "Tile", 15);

    m_selectionModesButtons.push_back(tileCornerMode);
    m_selectionModesButtons.push_back(tileMode);

    for (int i = 0; i < m_selectionModesButtons.size(); i++) {
        UIFactory::applyDefaultSpriteButtonStyle(m_selectionModesButtons[i]);
        m_selectionModesButtons[i]->initOnClickCallback([this, i] () {
            this->setSelectionMode(i);
        });
    }
}

void SetHeightTool::initSetHeightUI(sf::Vector2f startMenuPosition)
{
    m_setHeightText = UIFactory::createText(startMenuPosition + sf::Vector2f(20, 180),"Enter height value", 15);
    UIFactory::applyDefaultTextStyle(m_setHeightText, UIFactory::TextVariant::Label);

    m_setHeightInput = UIFactory::createTextInput(startMenuPosition + sf::Vector2f(20, 210), {150, 40}, MathUtils::toString(m_heightValue), 18, true);
    m_setHeightInput->initCharacterLimit(7);
    m_setHeightInput->setAllowFloatInput(true);
    m_setHeightInput->setAllowNegativeInput(true);
    UIFactory::applyDefaultTextInputStyle(m_setHeightInput);
}

void SetHeightTool::initWidgetsList()
{
    m_widgets.push_back(m_setHeightToolBox);
    m_widgets.push_back(m_setHeightToolText);
    m_widgets.push_back(m_selectionModeBox);
    m_widgets.push_back(m_selectionModeText);
    for (SpriteButton *button : m_selectionModesButtons)
        m_widgets.push_back(button);
    m_widgets.push_back(m_setHeightInput);
    m_widgets.push_back(m_setHeightText);
}