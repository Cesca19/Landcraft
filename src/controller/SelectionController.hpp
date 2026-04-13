//
// Created by fran on 13/03/2026.
//

#ifndef LANDCRAFT_SELECTIONCONTROLLER_HPP
#define LANDCRAFT_SELECTIONCONTROLLER_HPP

#include <set>
#include <SFML/Graphics.hpp>
#include "../model/WorldModel.hpp"
#include "../view/SelectionView.hpp"
#include "../view/Camera.hpp"
#include "../ui/UIFactory.hpp"

enum class SelectionMode {
    TILE,
    TILE_CORNER
};

class SelectionController {
public:
    SelectionController(sf::Vector2f uiStartPosition);
    ~SelectionController();
    void update(float deltaTime, const sf::RenderWindow &window, SelectionMode selectionMode,
                 WorldModel &worldModel, const Camera &camera);
    void draw(sf::RenderWindow &window, const Camera &camera);

    std::vector<TileCorner *> getSelectedTileCorners() const;
    const std::vector<Tile *> &getSelectedTiles() const;
    sf::Vector2i getMouseWorldPosition() const;
    bool isAnyTileCornerSelected() const;
private:
    void getSelectedCorners(const sf::RenderWindow &window, const Camera &camera, WorldModel &worldModel, SelectionMode selectionMode);
    void getSelectedTilesCorners(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void getSelectedTiles(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    int getSearchRadius(const Camera &camera, const WorldModel &worldModel) const;
    TileCorner *getClosestNeighborCornerInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition, 
                                                sf::Vector2f pointScreenPosition, int radius) const;
    std::vector<TileCorner *> getPointNeighborsInRadius(const Camera &camera, WorldModel &worldModel, int x, int y, int radius) const;

    Tile *getSelectedTileInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius) const;
    std::vector<Tile *> getClosestTilesInRadius(const Camera &camera, WorldModel &worldModel, int x, int y, int radius) const;
    bool isPointInsideTile(const Camera &camera, Tile *tile, sf::Vector2f pointScreenPosition) const;
    sf::Vector2f getTileCornerScreenCoordinates(const Camera &camera, const TileCorner* corner) const;

    void initBrushSizeWidgets();
    void initButtonStyle(SpriteButton *button, HighlightTextAlign align = HighlightTextAlign::Top);
    void initWidgetsList();

    SelectionView m_selectionView;
    std::vector<TileCorner *> m_selectedTileCorners;
    std::vector<Tile *> m_selectedTiles;
    sf::Vector2i m_mouseWorldPosition;

    sf::Vector2f m_startUIPosition;
    Box *m_brushSizeBox;
    Text *m_brushSizeText;
    Text *m_brushSizeValueText;
    SpriteButton *m_incrementBrushSize;
    SpriteButton *m_decrementBrushSize;
    std::vector<IWidget *> m_widgets;
};

#endif //LANDCRAFT_SELECTIONCONTROLLER_HPP
