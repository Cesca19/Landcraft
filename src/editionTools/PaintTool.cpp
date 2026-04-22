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
    m_paintToolBox = UIFactory::createBox(startMenuPosition, {110, 350});
    UIFactory::applyDefaultBoxStyle(m_paintToolBox);

    m_paintToolText = UIFactory::createText(startMenuPosition + sf::Vector2f(7.5, 10),"Textures", 20);
    m_paintToolText->init(sf::Color(123, 101, 81), sf::Text::Bold | sf::Text::Underlined);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(25, 55);
    SpriteButton *clearTextureButton = UIFactory::createSpriteButton("assets/textures/ui/clear_512.png", startBtnPosition + sf::Vector2f(0, 225),
        sf::Vector2f(32, 32), "Clear", 15);
    SpriteButton *grassTextureButton = UIFactory::createSpriteButton("assets/textures/ui/grass_512.png", startBtnPosition,
        sf::Vector2f(32, 32), "Grass", 15);
    SpriteButton *waterTextureButton = UIFactory::createSpriteButton("assets/textures/ui/water_512.png", startBtnPosition + sf::Vector2f(0, 75),
        sf::Vector2f(32, 32), "Water", 15);
    SpriteButton *sandTextureButton = UIFactory::createSpriteButton("assets/textures/ui/sand_512.png", startBtnPosition + sf::Vector2f(0, 150),
        sf::Vector2f(32, 32), "Sand", 15);

    m_paintTextureButtons.push_back(clearTextureButton);
    m_paintTextureButtons.push_back(grassTextureButton);
    m_paintTextureButtons.push_back(waterTextureButton);
    m_paintTextureButtons.push_back(sandTextureButton);
    for (int i = 0; i < m_paintTextureButtons.size(); i++) {
        UIFactory::applyDefaultSpriteButtonStyle(m_paintTextureButtons[i]);
        m_paintTextureButtons[i]->initOnClickCallback([this, i] () {
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

void PaintTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, BrushController &brushController,
                             CommandHistory &history)
{
    // tile painting texture picking
    // Later it will we be ui button that will change the current textureId
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Num0 || event.key.code == sf::Keyboard::Numpad0) selectPaintTexture(0); // clear
        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) selectPaintTexture(1); // grass
        if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) selectPaintTexture(2); // water
        if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) selectPaintTexture(3); // sand
    }

    // // paint starting
    // if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == m_paintMouseButton
    //     && m_ongoingPaintCommand == nullptr) {
    //     m_ongoingPaintCommand = std::make_unique<PaintTilesCommand>(brushController.getSelectedTiles(), m_currentTextureId);
    //     m_ongoingPaintCommand->execute(model, view);
    //     m_previousMousePosition = brushController.getMouseWorldPosition();
    //     m_isEditing = true;
    // }
    // paint ending
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == m_paintMouseButton
        && m_ongoingPaintCommand != nullptr) {
        if (!m_ongoingPaintCommand->isEmpty())
            history.addCommand(std::move(m_ongoingPaintCommand), model, view);
        m_ongoingPaintCommand = nullptr;
        m_previousMousePosition = {-1, -1};
        m_isEditing = false;
    }
}

void PaintTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController,
    CommandHistory &history)
{
    // // tiles painting
    // const std::vector<Tile *>& selectedTiles = brushController.getSelectedTiles();
    // if (!sf::Mouse::isButtonPressed(m_paintMouseButton)
    //     || m_ongoingPaintCommand == nullptr
    //     || selectedTiles.empty())
    //     return;

    // const sf::Vector2i currentMousePosition = brushController.getMouseWorldPosition();
    // if (currentMousePosition == m_previousMousePosition)
    //     return;

    // if (m_previousMousePosition == sf::Vector2i{-1, -1}) {
    //     m_ongoingPaintCommand->AddTiles(selectedTiles, model, view);
    //     m_previousMousePosition = currentMousePosition;
    //     return;
    // }

    // std::vector<std::vector<Tile>> &worldTiles = model.getTiles();
    // if (worldTiles.empty() || worldTiles[0].empty())
    //     return;

    // const std::vector<sf::Vector2i> lineTilesPositions =
    //         MathUtils::getBresenhamLine(m_previousMousePosition, currentMousePosition);
    // for (const sf::Vector2i& pos : lineTilesPositions)
    //     if (pos.y >= 0 && pos.y < static_cast<int>(worldTiles.size())
    //     && pos.x >= 0 && pos.x < static_cast<int>(worldTiles[0].size())) {
    //         std::vector<Tile *> tilesInBrush = brushController.getNeighborsTilesInBrush(model, pos.x, pos.y);
    //         m_ongoingPaintCommand->AddTiles(tilesInBrush, model, view);
    //     }
    // m_previousMousePosition = currentMousePosition;
}

void PaintTool::selectPaintTexture(const int textureId)
{
    if (textureId == m_currentTextureId
        || textureId < 0
        || textureId > 3)
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
