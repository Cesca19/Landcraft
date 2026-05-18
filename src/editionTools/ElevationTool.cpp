//
// Created by fran on 24/03/2026.
//

#include "ElevationTool.hpp"

ElevationTool::ElevationTool(const sf::Vector2f startMenuPosition)
    : m_shouldDig(false)
    , m_shouldElevate(false)
    , m_heightStep(1)
    , m_heightStepFactor(1)
    , m_maxHeightStepFactor(10)
    , m_isEditing(false)
    , m_isSelectionLocked(false)
    , m_currentSelectionMode(-1)
    , m_continuousElevationInterval(0.25f)
    , m_lastMouseScreenPosition(-1, -1)
    , m_lastMouseWorldPosition(-1, -1)
    , m_mouseMovementThreshold(2.5f)
    , m_ongoingEditCornersHeightCommand(nullptr)
    , m_elevationStepIncrement(nullptr)
    , m_elevationStepDecrement(nullptr)
    , m_digButton(nullptr)
    , m_elevateButton(nullptr)
    , m_elevationToolBox(nullptr)
    , m_selectionModeBox(nullptr)
    , m_elevationStepBox(nullptr)
    , m_digOrElevateBox(nullptr)
    , m_elevationToolText(nullptr)
    , m_selectionModeText(nullptr)
    , m_elevationStepText(nullptr)
    , m_elevationStepValueText(nullptr)
    , m_digOrElevateText(nullptr)
{
    m_elevationToolBox = UIFactory::createBox(startMenuPosition, {190, 470});
    UIFactory::applyDefaultBoxStyle(m_elevationToolBox);

    m_elevationToolText = UIFactory::createText(startMenuPosition + sf::Vector2f(50, 10),"Elevation", 20);
    UIFactory::applyDefaultTextStyle(m_elevationToolText, UIFactory::TextVariant::Title);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(35, 100);
    initSelectionModeUI(startMenuPosition + sf::Vector2f(0, 10));
    initDigOrElevateUI(startMenuPosition + sf::Vector2f(0, 0), startBtnPosition + sf::Vector2f(0, -5));
    initElevationStepUI(startMenuPosition + sf::Vector2f(0, 140), startBtnPosition + sf::Vector2f(0, 150));
    initToolWidgetsList();
    setUIVisibility(false);

    m_selectionModes.push_back(SelectionMode::TILE_CORNER);
    m_selectionModes.push_back(SelectionMode::TILE);
    setSelectionMode(0);
    elevate();
}

ElevationTool::~ElevationTool()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

bool ElevationTool::isEditing() const
{
    return m_isEditing;
}

bool ElevationTool::isSelectionLocked() const
{
    return m_isSelectionLocked;
}

SelectionMode ElevationTool::getRequiredSelectionMode() const {
    return m_selectionModes[m_currentSelectionMode];
}

void ElevationTool::onToolSelected() const
{
    setUIVisibility(true);
}

void ElevationTool::onToolUnSelected() const
{
    setUIVisibility(false);
}

bool ElevationTool::areEditableTilesVisible() const
{
    return true;
}

void ElevationTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view,
                                 BrushController &brushController, CommandHistory &history)
{
    handleSelectionModeEditingEvents(event);
    handleHeightStepEditingEvents(event);
}

void ElevationTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController,
                                           CommandHistory &history)
{
    handleHeightEditingEvents(window, model, view, brushController, history);
}

void ElevationTool::handleSelectionModeEditingEvents(const sf::Event &event)
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

void ElevationTool::handleHeightStepEditingEvents(const sf::Event &event)
{
    // to remove
    // if (event.type == sf::Event::KeyPressed) {
    //     if (event.key.code == sf::Keyboard::P)
    //        incrementHeightStepFactor();
    //     if (event.key.code == sf::Keyboard::M)
    //         decrementHeightStepFactor();
    // }
}

void ElevationTool::handleHeightEditingEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view,
                                              const BrushController &brushController, CommandHistory &history)
{
    bool isMouseButtonPressed = sf::Mouse::isButtonPressed(m_editingMouseButton);

    if ((m_shouldElevate || m_shouldDig) && isMouseButtonPressed) {
        const float heightFactor = (m_shouldElevate ? 1.0f : -1.0f ) * m_heightStep * static_cast<float>(m_heightStepFactor);
        if (m_ongoingEditCornersHeightCommand == nullptr)
            startContinuousElevation(window, model, view, brushController, heightFactor);
        else
            updateContinuousElevation(window, model, view, brushController, heightFactor);
        m_isEditing = true;
    } else {
        if (m_ongoingEditCornersHeightCommand != nullptr)
            stopContinuousElevation(model, view, history);
        m_isEditing = false;
    }
}

void ElevationTool::startContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view,
    const BrushController &brushController, const float heightStep)
{
    const std::vector<BrushTileCornerHit> &brushSelection = (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER) ? brushController.getBrushTileCornersSelection() : brushController.getBrushTilesSelectionAsTileCorners();
    if (brushSelection.empty()) return;
    m_ongoingEditCornersHeightCommand = std::make_unique<EditTilesCornersHeightCommand>(/*selectedCorners, heightStep*/);
    m_ongoingEditCornersHeightCommand->addCorners(brushSelection, heightStep, model, view);
    m_isSelectionLocked = true;
    m_continuousElevationClock.restart();
    m_lastMouseScreenPosition = sf::Mouse::getPosition(window);
    m_lastMouseWorldPosition = brushController.getMouseWorldPosition();
}

void ElevationTool::updateContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view,
    const BrushController &brushController, const float heightStep)
{
    if (m_ongoingEditCornersHeightCommand == nullptr)
        return;
    const sf::Vector2i currentMouseScreenPosition = sf::Mouse::getPosition(window);
    const bool hasScreenMouseMoved = MathUtils::distanceBetweenPoints(static_cast<sf::Vector2f>(m_lastMouseScreenPosition),
                                static_cast<sf::Vector2f>(currentMouseScreenPosition)) > m_mouseMovementThreshold;
    const sf::Vector2i currentMouseWorldPosition = brushController.getMouseWorldPosition();
    const bool hasAnyMouseMoved = hasScreenMouseMoved || view.isMoving();

    if (!hasAnyMouseMoved) {
        m_isSelectionLocked = true;
        if (m_continuousElevationClock.getElapsedTime().asSeconds() >= m_continuousElevationInterval) {
            applyElevationOnCurrentSelection(model, view, brushController, heightStep);
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
        applyElevationAlongPath(currentMouseWorldPosition, model, view, brushController, heightStep);
        m_lastMouseWorldPosition = currentMouseWorldPosition;
        m_lastMouseScreenPosition = currentMouseScreenPosition;
        m_continuousElevationClock.restart();
        m_isSelectionLocked = true;
    } else if (m_continuousElevationClock.getElapsedTime().asSeconds() >= m_continuousElevationInterval) {
        applyElevationOnCurrentSelection(model, view, brushController, heightStep);
        m_continuousElevationClock.restart();
        m_lastMouseScreenPosition = currentMouseScreenPosition;
        m_isSelectionLocked = true;
    } else
        m_isSelectionLocked = false;
}

void ElevationTool::applyElevationOnCurrentSelection(WorldModel &model, const WorldView &view,
    const BrushController &brushController, const float heightStep) const
{
    const std::vector<BrushTileCornerHit> &brushSelection =  (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER) ? brushController.getBrushTileCornersSelection() : brushController.getBrushTilesSelectionAsTileCorners();
    if (brushSelection.empty()) return;
    m_ongoingEditCornersHeightCommand->addCorners(brushSelection, heightStep, model, view);
}

void ElevationTool::applyElevationAlongPath(const sf::Vector2i &currentWorldPosition, WorldModel &model,
    const WorldView &view, const BrushController &brushController, const float heightStep) const
{
    const std::vector<sf::Vector2i> lineTilesPositions = MathUtils::getBresenhamLine(m_lastMouseWorldPosition, currentWorldPosition);

    for (size_t i = 1; i < lineTilesPositions.size(); ++i) {
        const sf::Vector2i& pos = lineTilesPositions[i];
        std::vector<BrushTileCornerHit> stepSelection;
        if (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER)
            stepSelection = brushController.getNeighborsTileCornersInBrush(model, pos.x, pos.y);
        else
            stepSelection = brushController.getNeighborsTilesInBrushAsTileCorners(model, pos.x, pos.y);
        if (!stepSelection.empty())
            m_ongoingEditCornersHeightCommand->addCorners(stepSelection, heightStep, model, view);
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


std::unordered_map<TileCorner *, float> ElevationTool::getTilesCornersFromBresenhamLine(const sf::Vector2i startPosition,
    const sf::Vector2i endPosition, WorldModel &model, const BrushController &brushController) const
{
    std::unordered_map<TileCorner *, float> cornersToElevate = {};
    if (m_selectionModes[m_currentSelectionMode] == SelectionMode::TILE_CORNER) {
        const std::vector<std::vector<std::unique_ptr<TileCorner>>> &worldTilesCorners = model.getCorners();
        if (worldTilesCorners.empty() || worldTilesCorners[0].empty())
            return cornersToElevate;
        const std::vector<sf::Vector2i> lineTilesPositions =
                MathUtils::getBresenhamLine(startPosition, endPosition);
        for (const sf::Vector2i& pos : lineTilesPositions)
            if (pos.y >= 0 && pos.y < static_cast<int>(worldTilesCorners.size())
            && pos.x >= 0 && pos.x < static_cast<int>(worldTilesCorners[0].size())) {
                std::vector<BrushTileCornerHit> brushSelection = brushController.getNeighborsTileCornersInBrush(model, pos.x, pos.y);
                for (const auto &[corner, weight] : brushSelection) {
                    if (cornersToElevate.find(corner) == cornersToElevate.end()
                        || cornersToElevate[corner] < weight)
                         cornersToElevate[corner] = weight;
                }
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
                std::vector<BrushTileCornerHit> brushSelection = brushController.getNeighborsTilesInBrushAsTileCorners(model, pos.x, pos.y);
                for (const auto &[corner, weight] : brushSelection)
                    if (cornersToElevate.find(corner) == cornersToElevate.end()
                        || cornersToElevate[corner] < weight)
                        cornersToElevate[corner] = weight;
            }
    }
    return cornersToElevate;
}

void ElevationTool::setUIVisibility(const bool isVisible) const
{
    for (const auto widget : m_widgets)
        widget->setVisibility(isVisible);
}

void ElevationTool::initSelectionModeUI(const sf::Vector2f startMenuPosition)
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

void ElevationTool::initDigOrElevateUI(sf::Vector2f startMenuPosition, sf::Vector2f startButtonPosition)
{
    m_digOrElevateBox = UIFactory::createBox(startMenuPosition + sf::Vector2f(20, 200), {150, 125});
    UIFactory::applyDefaultBoxStyle(m_digOrElevateBox);

    m_digOrElevateText = UIFactory::createText(startMenuPosition + sf::Vector2f(35, 210),"Action", 15);
    UIFactory::applyDefaultTextStyle(m_digOrElevateText, UIFactory::TextVariant::Label);

    m_digButton = UIFactory::createSpriteButton("assets/textures/ui/dig_512.png", startButtonPosition + sf::Vector2f(65, 155),
        sf::Vector2f(32, 32), "Dig", 15);
    m_elevateButton = UIFactory::createSpriteButton("assets/textures/ui/elevate_512.png", startButtonPosition + sf::Vector2f(0, 155),
        sf::Vector2f(32, 32), "Elevate", 15);

    UIFactory::applyDefaultSpriteButtonStyle(m_digButton);
    UIFactory::applyDefaultSpriteButtonStyle(m_elevateButton);

    m_digButton->initOnClickCallback([this] () {
        this->dig();
    });
    m_elevateButton->initOnClickCallback([this] () {
        this->elevate();
    });
}

void ElevationTool::initElevationStepUI(const sf::Vector2f startMenuPosition, const sf::Vector2f startButtonPosition)
{
    m_elevationStepBox = UIFactory::createBox(startMenuPosition + sf::Vector2f(20, 200), {150, 110});
    UIFactory::applyDefaultBoxStyle(m_elevationStepBox);

    m_elevationStepText = UIFactory::createText(startMenuPosition + sf::Vector2f(35, 210),"Height Step", 15);
    UIFactory::applyDefaultTextStyle(m_elevationStepText, UIFactory::TextVariant::Label);

    m_elevationStepValueText = UIFactory::createText(startButtonPosition + sf::Vector2f(47, 145), getHeightStepValue(), 20);
    UIFactory::applyDefaultTextStyle(m_elevationStepValueText, UIFactory::TextVariant::Value);

    m_elevationStepDecrement = UIFactory::createSpriteButton("assets/textures/ui/reduce_512.png", startButtonPosition + sf::Vector2f(0, 140),
        sf::Vector2f(20, 20), "Reduce", 15);
    m_elevationStepIncrement = UIFactory::createSpriteButton("assets/textures/ui/add_512.png", startButtonPosition + sf::Vector2f(80, 140),
        sf::Vector2f(20, 20), "Add", 15);

    m_elevationStepDecrement->setContinuousClick(true, 0.5);
    m_elevationStepIncrement->setContinuousClick(true, 0.5);

    UIFactory::applyDefaultSpriteButtonStyle(m_elevationStepDecrement);
    UIFactory::applyDefaultSpriteButtonStyle(m_elevationStepIncrement);

    m_elevationStepDecrement->initOnClickCallback([this] () {
        this->decrementHeightStepFactor();
    });
    m_elevationStepIncrement->initOnClickCallback([this] () {
        this->incrementHeightStepFactor();
    });
}

void ElevationTool::initToolWidgetsList()
{
    for (SpriteButton *button : m_selectionModesButtons)
        m_widgets.push_back(button);
    m_widgets.push_back(m_elevationStepDecrement);
    m_widgets.push_back(m_elevationStepIncrement);
    m_widgets.push_back(m_elevationToolBox);
    m_widgets.push_back(m_selectionModeBox);
    m_widgets.push_back(m_elevationStepBox);
    m_widgets.push_back(m_elevationToolText);
    m_widgets.push_back(m_selectionModeText);
    m_widgets.push_back(m_elevationStepText);
    m_widgets.push_back(m_elevationStepValueText);
    m_widgets.push_back(m_digOrElevateBox);
    m_widgets.push_back(m_digOrElevateText);
    m_widgets.push_back(m_digButton);
    m_widgets.push_back(m_elevateButton);
}

std::string ElevationTool::getHeightStepValue() const
{
    std::string value = std::to_string(static_cast<int>(m_heightStep) * m_heightStepFactor);
    return (value.size() > 1) ? value : "0" + value;
}

void ElevationTool::setSelectionMode(const int index)
{
    if (index < 0 || index >= static_cast<int>(m_selectionModes.size()) || index == m_currentSelectionMode)
        return;
    if (m_currentSelectionMode != -1)
        m_selectionModesButtons[m_currentSelectionMode]->setSelected(false);
    m_currentSelectionMode = index;
    m_selectionModesButtons[m_currentSelectionMode]->setSelected(true);
}

void ElevationTool::incrementHeightStepFactor()
{
    updateHeightStepFactor(m_heightStepFactor + 1);
}

void ElevationTool::decrementHeightStepFactor()
{
    updateHeightStepFactor(m_heightStepFactor - 1);
}

void ElevationTool::updateHeightStepFactor(const int newValue)
{
    if (newValue == m_heightStepFactor || m_heightStepFactor == std::clamp(newValue, 1, m_maxHeightStepFactor))
        return;
    m_heightStepFactor = std::clamp(newValue, 1, m_maxHeightStepFactor);
    m_elevationStepValueText->setContent(getHeightStepValue());
}

void ElevationTool::dig()
{
    if (m_shouldElevate) {
        m_elevateButton->setSelected(false);
        m_shouldElevate = false;
    }
    m_digButton->setSelected(true);
    m_shouldDig = true;
}

void ElevationTool::elevate()
{
    if (m_shouldDig) {
        m_digButton->setSelected(false);
        m_shouldDig = false;
    }
    m_elevateButton->setSelected(true);
    m_shouldElevate = true;
}