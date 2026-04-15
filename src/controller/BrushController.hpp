//
// Created by fran on 13/03/2026.
//

#ifndef LANDCRAFT_BRUSHONTROLLER_HPP
#define LANDCRAFT_BRUSHONTROLLER_HPP

#include <set>
#include <SFML/Graphics.hpp>
#include "../model/WorldModel.hpp"
#include "../view/world/BrushView.hpp"
#include "../view/menu/BrushMenu.hpp"
#include "../view/world/Camera.hpp"

enum class SelectionMode {
    TILE,
    TILE_CORNER
};

class BrushController {
public:
    BrushController(sf::Vector2f uiStartPosition);
    void update(float deltaTime, const sf::RenderWindow &window, SelectionMode selectionMode,
                 WorldModel &worldModel, const Camera &camera);
    void draw(sf::RenderWindow &window, const Camera &camera);

    std::vector<TileCorner *> getSelectedTileCorners() const;
    const std::vector<Tile *> &getSelectedTiles() const;
    sf::Vector2i getMouseWorldPosition() const;
    bool isAnyTileCornerSelected() const;
    std::vector<Tile *> getNeighborsTilesInBrush(WorldModel &worldModel, int x, int y) const;
private:
    void getSelectedCorners(const sf::RenderWindow &window, const Camera &camera, WorldModel &worldModel, SelectionMode selectionMode);
    void getSelectedTilesCorners(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void getSelectedTiles(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    int getSearchRadius(const Camera &camera, const WorldModel &worldModel) const;
    TileCorner *getClosestNeighborCornerInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition,
                                                sf::Vector2f pointScreenPosition, int radius) const;
    std::vector<TileCorner *> getPointNeighborsInRadius(WorldModel &worldModel, int x, int y, int radius) const;

    Tile *getSelectedTileInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius) const;
    std::vector<Tile *> getClosestTilesInRadius(WorldModel &worldModel, int x, int y, int radius, bool includeInside = false) const;
    bool isPointInsideTile(const Camera &camera, Tile *tile, sf::Vector2f pointScreenPosition) const;
    sf::Vector2f getTileCornerScreenCoordinates(const Camera &camera, const TileCorner* corner) const;

    void incrementBrushSize();
    void decrementBrushSize();
    std::string getBrushSizeValue() const;

    BrushView m_brushView;
    BrushMenu m_brushMenu;

    std::vector<TileCorner *> m_selectedTileCorners;
    std::vector<Tile *> m_selectedTiles;
    sf::Vector2i m_mouseWorldPosition;

    float m_brushSize;
    float m_brushSizeMin;
    float m_brushSizeMax;
};

#endif //LANDCRAFT_BRUSHONTROLLER_HPP
