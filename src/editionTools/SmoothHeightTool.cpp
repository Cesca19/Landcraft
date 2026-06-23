//
// Created by fran on 23/06/2026.
//

#include "SmoothHeightTool.hpp"

SmoothHeightTool::SmoothHeightTool(sf::Vector2f startMenuPosition)
    : m_isEditing(false)
    , m_isSelectionLocked(false)
    , m_currentSelectionMode(-1)
    , m_lastMouseScreenPosition(-1, -1)
    , m_lastMouseWorldPosition(-1, -1)
    , m_mouseMovementThreshold(0.5f)
    , m_ongoingSetTilesCornersHeightCommand(nullptr)
    , m_smoothStrength(0.4f)
    , m_continuousElevationInterval(0.1f)
    , m_smoothStrengthStep(0.05f)
    , m_minSmoothStrength(0.05f)
    , m_maxSmoothStrength(1.0f)
{
    m_selectionModes.push_back(SelectionMode::TILE_CORNER);
    m_selectionModes.push_back(SelectionMode::TILE);
    initMenuUI(startMenuPosition);
    setUIVisibility(false);
    setSelectionMode(0);
}

SmoothHeightTool::~SmoothHeightTool()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

bool SmoothHeightTool::isEditing() const
{
    return m_isEditing;
}

bool SmoothHeightTool::isSelectionLocked() const
{
    return m_isSelectionLocked;
}

SelectionMode SmoothHeightTool::getRequiredSelectionMode() const
{
    return m_selectionModes[m_currentSelectionMode];
}

void SmoothHeightTool::onToolSelected() const
{
    setUIVisibility(true);
}

void SmoothHeightTool::onToolUnSelected() const
{
    setUIVisibility(false);
}

bool SmoothHeightTool::areEditableTilesVisible() const
{
    return true;
}

void SmoothHeightTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view,
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

void SmoothHeightTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController,
                                               CommandHistory &history)
{
    bool isMouseButtonPressed = sf::Mouse::isButtonPressed(m_editingMouseButton);

    if (isMouseButtonPressed) {
        if (m_ongoingSetTilesCornersHeightCommand == nullptr)
            startContinuousSmoothing(window, model, view, brushController);
        else
            updateContinuousSmoothing(window, model, view, brushController);
        m_isEditing = true;
    } else {
        if (m_ongoingSetTilesCornersHeightCommand != nullptr)
            stopContinuousSmoothing(model, view, history);
        m_isEditing = false;
    }
}

void SmoothHeightTool::setVisibility(bool isVisible) const
{
    setUIVisibility(isVisible);
}

void SmoothHeightTool::startContinuousSmoothing(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const BrushController &brushController)
{
    const std::vector<BrushTileCornerHit> &brushSelection = (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER) ? brushController.getBrushTileCornersSelection() : brushController.getBrushTilesSelectionAsTileCorners();
    if (brushSelection.empty()) return;
    m_ongoingSetTilesCornersHeightCommand = std::make_unique<SetTilesCornersHeightCommand>();
    applySmoothingOnCurrentSelection(model, view, brushController);
    m_isSelectionLocked = true;
    m_continuousElevationClock.restart();
    m_lastMouseScreenPosition = sf::Mouse::getPosition(window);
    m_lastMouseWorldPosition = brushController.getMouseWorldPosition();
}

void SmoothHeightTool::updateContinuousSmoothing(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const BrushController &brushController)
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
        if (m_continuousElevationClock.getElapsedTime().asSeconds() >= m_continuousElevationInterval) {
            applySmoothingOnCurrentSelection(model, view, brushController);
            m_continuousElevationClock.restart();
        }
        return;
    }
    // here the mouse is moving
    if (m_isSelectionLocked) { // wait for the next frame to let selection controller upd
        m_isSelectionLocked = false;
        return;
    }
    if (m_lastMouseWorldPosition != currentMouseWorldPosition) {
        applySmoothingAlongPath(currentMouseWorldPosition, model, view, brushController);
        m_lastMouseWorldPosition = currentMouseWorldPosition;
        m_lastMouseScreenPosition = currentMouseScreenPosition;
        m_continuousElevationClock.restart();
        m_isSelectionLocked = true;
    } else if (m_continuousElevationClock.getElapsedTime().asSeconds() >= m_continuousElevationInterval) {
        applySmoothingOnCurrentSelection(model, view, brushController);
        m_continuousElevationClock.restart();
        m_lastMouseScreenPosition = currentMouseScreenPosition;
        m_isSelectionLocked = true;
    } else
        m_isSelectionLocked = false;
}

void SmoothHeightTool::stopContinuousSmoothing(WorldModel &model, WorldView &view, CommandHistory &history)
{
    if (m_ongoingSetTilesCornersHeightCommand == nullptr)
        return;
    history.addCommand(std::move(m_ongoingSetTilesCornersHeightCommand), model, view);
    m_ongoingSetTilesCornersHeightCommand = nullptr;
    m_isSelectionLocked = false;
    m_continuousElevationClock.restart();
    m_lastMouseScreenPosition = sf::Vector2i(-1, -1);
}

void SmoothHeightTool::applySmoothingOnCurrentSelection(WorldModel &model, const WorldView &view, const BrushController &brushController)
{
    const std::vector<BrushTileCornerHit> &brushSelection =  (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER) ? brushController.getBrushTileCornersSelection() : brushController.getBrushTilesSelectionAsTileCorners();
    if (brushSelection.empty()) 
        return;

    std::unordered_map<TileCorner*, float> targetHeights = computeSmoothHeights(brushSelection, model);
    if (m_ongoingSetTilesCornersHeightCommand != nullptr && !targetHeights.empty())
        m_ongoingSetTilesCornersHeightCommand->addCorners(targetHeights, model, view);
}

void SmoothHeightTool::applySmoothingAlongPath(sf::Vector2i targetPosition, WorldModel &model, const WorldView &view, const BrushController &brushController)
{
    const std::vector<sf::Vector2i> lineTilesPositions = MathUtils::getBresenhamLine(m_lastMouseWorldPosition, targetPosition);

    for (size_t i = 1; i < lineTilesPositions.size(); ++i) {
        const sf::Vector2i& pos = lineTilesPositions[i];
        
        // Get the brush selection for the current step on the Bresenham line
        std::vector<BrushTileCornerHit> stepSelection;
        if (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER)
            stepSelection = brushController.getNeighborsTileCornersInBrush(model, pos.x, pos.y);
        else
            stepSelection = brushController.getNeighborsTilesInBrushAsTileCorners(model, pos.x, pos.y);

        if (stepSelection.empty())
            continue;
        // Calculate the target heights for this specific step using the helper function
        std::unordered_map<TileCorner*, float> targetHeights = computeSmoothHeights(stepSelection, model);
        // Apply the changes immediately for this step to create a seamless dragging effect
        if (m_ongoingSetTilesCornersHeightCommand != nullptr && !targetHeights.empty()) {
            m_ongoingSetTilesCornersHeightCommand->addCorners(targetHeights, model, view);
        }
    }
}

std::unordered_map<TileCorner *, float> SmoothHeightTool::computeSmoothHeights(const std::vector<BrushTileCornerHit> &selection,  WorldModel &model) const
{
    std::unordered_map<TileCorner*, float> targetHeights;
    const std::vector<std::vector<std::unique_ptr<TileCorner>>>& corners = model.getCorners();
    int maxRow = corners.size();
    int maxCol = maxRow > 0 ? corners[0].size() : 0;

    for (const BrushTileCornerHit& hit : selection) {
        TileCorner* corner = hit.corner;
        float brushWeight = hit.weight;
        int row = static_cast<int>(corner->getRow());
        int col = static_cast<int>(corner->getColumn());
        float sumHeight = 0.0f;
        int neighborCount = 0;

        // Gather heights from the 3x3 neighborhood
        for (int r = row - 1; r <= row + 1; ++r) {
            for (int c = col - 1; c <= col + 1; ++c) {
                // Check map boundaries
                if (r >= 0 && r < maxRow && c >= 0 && c < maxCol) {
                    sumHeight += corners[r][c]->getHeight();
                    neighborCount++;
                }
            }
        }
        // Calculate the local average and interpolate the new height
        float averageHeight = sumHeight / static_cast<float>(neighborCount);
        float currentHeight = corner->getHeight();
        // Lerp towards the average height based on tool strength and brush weight
        float newHeight = currentHeight + ((averageHeight - currentHeight) * brushWeight * m_smoothStrength);
        targetHeights[corner] = newHeight;
    }
    return targetHeights;
}

std::string SmoothHeightTool::getSmoothStrengthValue() const
{
    std::string value = MathUtils::toString(m_smoothStrength);
    if (value.length() == 1)
        value += ".00";
    if (value.length() == 2)
        value += "0";
    if (value.length() == 3)
        value += "0";
    return value;
}

void SmoothHeightTool::incrementSmoothStrength()
{
    float newValue = m_smoothStrength + m_smoothStrengthStep;
    if (newValue > m_maxSmoothStrength)
        newValue = m_maxSmoothStrength;
    updateSmoothStrength(newValue);
}

void SmoothHeightTool::decrementSmoothStrength()
{
    float newValue = m_smoothStrength - m_smoothStrengthStep;
    if (newValue < m_minSmoothStrength)
        newValue = m_minSmoothStrength;
    updateSmoothStrength(newValue);
}

void SmoothHeightTool::updateSmoothStrength(float newValue)
{
    m_smoothStrength = MathUtils::roundToDecimalPlaces(newValue, 2);
    if (m_smoothValueText)
        m_smoothValueText->setContent(getSmoothStrengthValue());
}

void SmoothHeightTool::setSelectionMode(int index)
{
    if (index < 0 || index >= static_cast<int>(m_selectionModes.size()) || index == m_currentSelectionMode)
        return;
    if (m_currentSelectionMode != -1)
         m_selectionModesButtons[m_currentSelectionMode]->setSelected(false);
    m_currentSelectionMode = index;
    m_selectionModesButtons[m_currentSelectionMode]->setSelected(true);
}

void SmoothHeightTool::setUIVisibility(bool isVisible) const
{
    for (const auto widget : m_widgets)
        widget->setVisibility(isVisible);
}

void SmoothHeightTool::initMenuUI(sf::Vector2f startMenuPosition)
{
    m_smoothHeightToolBox = UIFactory::createBox(startMenuPosition, {220, 330});
    UIFactory::applyDefaultBoxStyle(m_smoothHeightToolBox);

    m_smoothHeightToolText = UIFactory::createText(startMenuPosition + sf::Vector2f(25, 10),"Smooth Height", 20);
    UIFactory::applyDefaultTextStyle(m_smoothHeightToolText, UIFactory::TextVariant::Title);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(35, 100);
    initSelectionModeUI(startMenuPosition + sf::Vector2f(0, 15));
    initSmoothStrengthUI(startMenuPosition + sf::Vector2f(0, 115));
    initWidgetsList();
}

void SmoothHeightTool::initSelectionModeUI(sf::Vector2f startMenuPosition)
{
    m_selectionModeBox = UIFactory::createBox(startMenuPosition + sf::Vector2f(20, 45), {180, 130});
    UIFactory::applyDefaultBoxStyle(m_selectionModeBox);

    m_selectionModeText =  UIFactory::createText(startMenuPosition + sf::Vector2f(35, 55),"Selection Mode", 15);
    UIFactory::applyDefaultTextStyle(m_selectionModeText, UIFactory::TextVariant::Label);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(35, 100);
    SpriteButton *tileCornerMode = UIFactory::createSpriteButton("assets/textures/ui/corner_512.png", startBtnPosition + sf::Vector2f(0, 0),
        sf::Vector2f(32, 32), "Corner", 15);
    SpriteButton *tileMode = UIFactory::createSpriteButton("assets/textures/ui/tiles_512.png", startBtnPosition + sf::Vector2f(95, 0),
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

void SmoothHeightTool::initSmoothStrengthUI(sf::Vector2f startMenuPosition)
{
    m_smoothStrengthBox = UIFactory::createBox(startMenuPosition + sf::Vector2f(20, 90), {180, 110});
    UIFactory::applyDefaultBoxStyle(m_smoothStrengthBox);

    m_smoothStrengthText = UIFactory::createText(startMenuPosition + sf::Vector2f(35, 100),"Smooth Strength", 15);
    UIFactory::applyDefaultTextStyle(m_smoothStrengthText, UIFactory::TextVariant::Label);


    const sf::Vector2f startButtonPosition = startMenuPosition + sf::Vector2f(35, 0);
    m_smoothValueText = UIFactory::createText(startButtonPosition + sf::Vector2f(52.5, 150), getSmoothStrengthValue(), 20);
    UIFactory::applyDefaultTextStyle(m_smoothValueText, UIFactory::TextVariant::Value);

    m_smoothStrengthDecreaseButton = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png", startButtonPosition + sf::Vector2f(0, 140),
        sf::Vector2f(20, 20), "Reduce", 15);
    m_smoothStrengthIncreaseButton = UIFactory::createSpriteButton("assets/textures/ui/add_512.png", startButtonPosition + sf::Vector2f(110, 140),
        sf::Vector2f(20, 20), "Add", 15);

    m_smoothStrengthDecreaseButton->setContinuousClick(true);
    m_smoothStrengthIncreaseButton->setContinuousClick(true);

    UIFactory::applyDefaultSpriteButtonStyle(m_smoothStrengthDecreaseButton);
    UIFactory::applyDefaultSpriteButtonStyle(m_smoothStrengthIncreaseButton);

    m_smoothStrengthDecreaseButton->initOnClickCallback([this] () {
        this->decrementSmoothStrength();
    });
    m_smoothStrengthIncreaseButton->initOnClickCallback([this] () {
        this->incrementSmoothStrength();
    });
}

void SmoothHeightTool::initWidgetsList()
{
    m_widgets.push_back(m_smoothHeightToolBox);
    m_widgets.push_back(m_smoothHeightToolText);
    m_widgets.push_back(m_selectionModeBox);
    m_widgets.push_back(m_selectionModeText);
    m_widgets.push_back(m_smoothStrengthBox);
    m_widgets.push_back(m_smoothStrengthText);
    m_widgets.push_back(m_smoothValueText);
    m_widgets.push_back(m_smoothStrengthDecreaseButton);
    m_widgets.push_back(m_smoothStrengthIncreaseButton);
    for (const auto button : m_selectionModesButtons)
        m_widgets.push_back(button);
}