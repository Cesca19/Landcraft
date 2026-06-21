//
// Created by fran on 24/03/2026.
//

#include "PaintTool.hpp"

PaintTool::PaintTool(const sf::Vector2f startMenuPosition)
    : m_isEditing(false)
    , m_currentTextureId(-1)
    , m_previousMousePosition(-1, -1)
    , m_ongoingPaintCommand(nullptr)
{
    m_paintToolBox = UIFactory::createBox(startMenuPosition, {110, 430});
    UIFactory::applyDefaultBoxStyle(m_paintToolBox);
    m_paintToolText = UIFactory::createText(startMenuPosition + sf::Vector2f(7.5, 10),"Textures", 20);
    UIFactory::applyDefaultTextStyle(m_paintToolText, UIFactory::TextVariant::Title);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(25, 55);
    SpriteButton *clearTextureButton = UIFactory::createSpriteButton("assets/textures/ui/clear_512.png", startBtnPosition + sf::Vector2f(0, 300),
        sf::Vector2f(32, 32), "Clear", 15);
    SpriteButton *grassTextureButton = UIFactory::createSpriteButton("assets/textures/grass_32.png", startBtnPosition,
        sf::Vector2f(32, 32), "Grass", 15);
    SpriteButton *waterTextureButton = UIFactory::createSpriteButton("assets/textures/sand_32.png", startBtnPosition + sf::Vector2f(0, 75),
        sf::Vector2f(32, 32), "Sand", 15);
    SpriteButton *sandTextureButton = UIFactory::createSpriteButton("assets/textures/rock_32.png", startBtnPosition + sf::Vector2f(0, 150),
        sf::Vector2f(32, 32), "Rock", 15);
    SpriteButton *snowTextureButton = UIFactory::createSpriteButton("assets/textures/snow_32.png", startBtnPosition + sf::Vector2f(0, 225),
        sf::Vector2f(32, 32), "Snow", 15);

    m_paintTextureButtons.push_back(clearTextureButton);
    m_paintTextureButtons.push_back(grassTextureButton);
    m_paintTextureButtons.push_back(waterTextureButton);
    m_paintTextureButtons.push_back(sandTextureButton);
    m_paintTextureButtons.push_back(snowTextureButton);
    for (int i = 0; i < m_paintTextureButtons.size(); i++) {
        UIFactory::applyDefaultSpriteButtonStyle(m_paintTextureButtons[i], HighlightTextAlign::Top, false);
        m_paintTextureButtons[i]->initOnClickCallback(
            [this, i] () {
            this->selectPaintTexture(i);
        });
    }
    selectPaintTexture(1);
    initToolWidgetsList();
    setUIVisibility(false);
}

PaintTool::~PaintTool()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
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

void PaintTool::onToolSelected() const
{
    setUIVisibility(true);
}

void PaintTool::onToolUnSelected() const
{
    setUIVisibility(false);
}

bool PaintTool::areEditableTilesVisible() const
{
    return false;
}

void PaintTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, BrushController &brushController,
                             CommandHistory &history)
{
    if (event.type == sf::Event::KeyPressed && !m_isEditing) {
        if (event.key.code == sf::Keyboard::Num0 || event.key.code == sf::Keyboard::Numpad0) selectPaintTexture(0); // clear
        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) selectPaintTexture(1); // grass
        if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) selectPaintTexture(2); // sand
        if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) selectPaintTexture(3); // rock
        if (event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Numpad4) selectPaintTexture(4); // snow
    }

    // paint starting
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == m_paintMouseButton
        && m_ongoingPaintCommand == nullptr) {
        // m_ongoingPaintCommand = std::make_unique<PaintTilesCommand>(m_currentTextureId);
        // m_ongoingPaintCommand->AddTiles(brushController.getBrushTilesSelection(), model, view);
        m_ongoingPaintCommand = std::make_unique<PaintSplatMapCommand>();
        m_previousMousePosition = brushController.getMouseWorldPosition();
        paintStroke(brushController.getBrushCenterWorldPosition(), model, view, 
            brushController.getCurrentBrushId(), brushController.getCurrentBrushRadius());
        m_isEditing = true;
    }
    // paint ending
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == m_paintMouseButton
        && m_ongoingPaintCommand != nullptr) {
        if (!m_ongoingPaintCommand->isEmpty()) {
            m_ongoingPaintCommand->execute(model, view);
            history.addCommand(std::move(m_ongoingPaintCommand), model, view);
        }
        m_ongoingPaintCommand = nullptr;
        m_previousMousePosition = {-1, -1};
        m_isEditing = false;
    }
}

void PaintTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController,
    CommandHistory &history)
{
    // splatmap painting
    const std::vector<BrushTileHit>& selectedTiles = brushController.getBrushTilesSelection();
    if (!sf::Mouse::isButtonPressed(m_paintMouseButton)
        || m_ongoingPaintCommand == nullptr) //|| selectedTiles.empty())
        return;
    const sf::Vector2i currentMousePosition = brushController.getMouseWorldPosition();
    // const sf::Vector2f currentMouseAbsolutePosition = brushController.getMouseAbsoluteWorldPosition();
    if (currentMousePosition == m_previousMousePosition)
        return;
    if (m_previousMousePosition == sf::Vector2i{-1, -1}) {
        paintStroke(brushController.getBrushCenterWorldPosition(), model, view, 
            brushController.getCurrentBrushId(), brushController.getCurrentBrushRadius());
        m_previousMousePosition = currentMousePosition;
        return;
    }
    const std::vector<sf::Vector2i> lineTilesPositions = MathUtils::getBresenhamLine(m_previousMousePosition, currentMousePosition);
    for (int i = 0; i < lineTilesPositions.size(); ++i) {
        const sf::Vector2i& pos = lineTilesPositions[i];
        paintStroke(sf::Vector2f(pos.x + 0.5f, pos.y + 0.5f), model, view, 
            brushController.getCurrentBrushId(), brushController.getCurrentBrushRadius());
    }
    m_previousMousePosition = currentMousePosition;
}

void PaintTool::setVisibility(bool isVisible) const
{
    for (const auto widget : m_widgets)
        widget->setVisibility(isVisible);
}

void PaintTool::paintStroke(const sf::Vector2f &position, WorldModel &model, WorldView &view, int brushId, int brushRadius)
{
    PaintStroke stroke;
    stroke.worldPosition = position;
    stroke.brushTextureId = brushId;
    stroke.textureId = m_currentTextureId;
    stroke.radius = brushRadius;
    m_ongoingPaintCommand->addStroke(stroke);
    m_ongoingPaintCommand->drawRealTime(model, view); 
}

void PaintTool::selectPaintTexture(const int textureId)
{
    if (textureId == m_currentTextureId
        || textureId < 0
        || textureId > 4)
        return;
    if (m_currentTextureId != -1)
        m_paintTextureButtons[m_currentTextureId]->setSelected(false);
    m_currentTextureId = textureId;
    m_paintTextureButtons[m_currentTextureId]->setSelected(true);
}

void PaintTool::setUIVisibility(const bool isVisible) const
{
    for (const auto widget : m_widgets)
        widget->setVisibility(isVisible);
}

void PaintTool::initToolWidgetsList()
{
    m_widgets.push_back(m_paintToolBox);
    m_widgets.push_back(m_paintToolText);
    for (const auto button : m_paintTextureButtons)
        m_widgets.push_back(button);
}
