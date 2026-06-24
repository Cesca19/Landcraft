//
// Created by fran on 24/06/2026.
//

#include "ResetTerrainTool.hpp"


ResetTerrainTool::ResetTerrainTool(sf::Vector2f startMenuPosition, WorldModel *model, WorldView *view, CommandHistory *history)
    : m_model(model)
    , m_view(view)
    , m_history(history)
{
    initMenuUI(startMenuPosition);
    initWidgetsList();
    setUIVisibility(false);

    m_resetSplatMapButton->initOnClickCallback([this] () {
        this->resetTerrainSplatMap(*m_model, *m_view, *m_history);
    });
    m_resetHeightButton->initOnClickCallback([this] () {
        this->resetTerrainHeight(*m_model, *m_view, *m_history);
    });
}

ResetTerrainTool::~ResetTerrainTool()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

bool ResetTerrainTool::isEditing() const
{
    return false;
}

bool ResetTerrainTool::isSelectionLocked() const
{
    return false;
}

SelectionMode ResetTerrainTool::getRequiredSelectionMode() const
{
    return SelectionMode::TILE;
}

void ResetTerrainTool::onToolSelected() const
{
    setUIVisibility(true);
}

void ResetTerrainTool::onToolUnSelected() const
{
    setUIVisibility(false);
}

bool ResetTerrainTool::areEditableTilesVisible() const
{
    return true;
}

void ResetTerrainTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, BrushController &brushController, CommandHistory &history)
{
}

void ResetTerrainTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController, CommandHistory &history)
{
}

void ResetTerrainTool::setVisibility(bool isVisible) const
{
    setUIVisibility(isVisible);
}

void ResetTerrainTool::resetTerrainSplatMap(WorldModel &model, WorldView &view, CommandHistory &history)
{
    sf::Image blankImage;
    sf::Image splatMapImage = view.getSplatmapImage();
    blankImage.create(splatMapImage.getSize().x, splatMapImage.getSize().y, sf::Color(0, 0, 0, 0));
    std::unique_ptr<SetSplatMapCommand> splatMapCommand = std::make_unique<SetSplatMapCommand>(view.getSplatmapImage(), blankImage);
    
    history.addCommand(std::move(splatMapCommand), model, view, true);
}

void ResetTerrainTool::resetTerrainHeight(WorldModel &model, WorldView &view, CommandHistory &history)
{
    std::vector<std::vector<float>> heightmap = getDefaultHeightmap(model);
    std::unique_ptr<SetTerrainHeightMapCommand> heightmapCommand = std::make_unique<SetTerrainHeightMapCommand>(heightmap);
    
    history.addCommand(std::move(heightmapCommand), model, view, true);
}

std::vector<std::vector<float>> ResetTerrainTool::getDefaultHeightmap(WorldModel &model) const
{
    const std::vector<std::vector<std::unique_ptr<TileCorner>>>& corners = model.getCorners();
    std::vector<std::vector<float>> defaultHeightmap(corners.size(), std::vector<float>(corners[0].size(), 0.0f));
    return defaultHeightmap;
}

void ResetTerrainTool::setUIVisibility(bool isVisible) const
{
    for (IWidget* widget : m_widgets)
        widget->setVisibility(isVisible);
}

void ResetTerrainTool::initMenuUI(sf::Vector2f startMenuPosition)
{
    m_resetToolBox = UIFactory::createBox(startMenuPosition, {170, 130});
    UIFactory::applyDefaultBoxStyle(m_resetToolBox);
    m_resetToolText = UIFactory::createText(startMenuPosition + sf::Vector2f(15, 10),"Reset Terrain", 20);
    UIFactory::applyDefaultTextStyle(m_resetToolText, UIFactory::TextVariant::Title);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(22.5, 60);
    m_resetHeightButton = UIFactory::createSpriteButton("assets/textures/ui/elevation_tool_512.png", startBtnPosition,
        sf::Vector2f(32, 32), "Height", 15);
    m_resetSplatMapButton = UIFactory::createSpriteButton("assets/textures/ui/paint_palette_64.png", startBtnPosition + sf::Vector2f(70, 0),
        sf::Vector2f(32, 32), "Paint", 15);
    UIFactory::applyDefaultSpriteButtonStyle(m_resetHeightButton, HighlightTextAlign::Top);
    UIFactory::applyDefaultSpriteButtonStyle(m_resetSplatMapButton, HighlightTextAlign::Top);
}

void ResetTerrainTool::initWidgetsList()
{
    m_widgets.push_back(m_resetToolBox);
    m_widgets.push_back(m_resetToolText);
    m_widgets.push_back(m_resetHeightButton);
    m_widgets.push_back(m_resetSplatMapButton);
}