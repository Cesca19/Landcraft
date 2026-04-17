//
// Created by fran on 13/03/2026.
//

#ifndef LANDCRAFT_BRUSHCONTROLLER_HPP
#define LANDCRAFT_BRUSHCONTROLLER_HPP

#include <set>
#include <SFML/Graphics.hpp>
#include "../model/WorldModel.hpp"
#include "../view/world/BrushView.hpp"
#include "../view/menu/BrushMenu.hpp"
#include "../view/world/Camera.hpp"
#include "../model/BrushInfos.hpp"


class BrushController {
public:
    BrushController(sf::Vector2f uiStartPosition);
    void handleEvents(const sf::RenderWindow &window, const sf::Event &event);
    void update(float deltaTime, const sf::RenderWindow &window, SelectionMode selectionMode,
                 WorldModel &worldModel, const Camera &camera);
    void draw(sf::RenderWindow &window, const Camera &camera);
    sf::Vector2i getMouseWorldPosition() const;
    bool isAnyTileCornerSelected() const;

    const std::vector<BrushTileCornerHit>& getBrushTileCornersSelection() const;
    const std::vector<BrushTileHit>& getBrushTilesSelection() const;
    const std::vector<BrushTileCornerHit>& getBrushTilesSelectionAsTileCorners() const;
    std::vector<BrushTileHit> getNeighborsTilesInBrush(WorldModel &worldModel, int x, int y) const;
    std::vector<BrushTileCornerHit> getNeighborsTileCornersInBrush(WorldModel &worldModel, int x, int y) const;
    std::vector<BrushTileCornerHit> getNeighborsTilesInBrushAsTileCorners(WorldModel &worldModel, int x, int y) const;
private:
    void getSelectedCorners(const sf::RenderWindow &window, const Camera &camera, WorldModel &worldModel, SelectionMode selectionMode);
    void getSelectedTilesCorners(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void getSelectedTiles(const Camera &camera, WorldModel &worldModel, sf::Vector2i mouseWorldPosition, sf::Vector2f mouseScreenPosition);
    void fillHoveredSelection(WorldModel &worldModel, SelectionMode selectionMode);
    int getSearchRadius(const Camera &camera, const WorldModel &worldModel) const;
    TileCorner *getClosestNeighborCornerInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition,
                                                sf::Vector2f pointScreenPosition, int radius) const;
    std::vector<TileCorner *> getPointNeighborsInRadius(WorldModel &worldModel, int x, int y, int radius) const;

    Tile *getSelectedTileInRadius(const Camera &camera, WorldModel &worldModel, sf::Vector2i pointWorldPosition, sf::Vector2f pointScreenPosition, int radius) const;
    std::vector<Tile *> getClosestTilesInRadius(WorldModel &worldModel, int x, int y, int radius, bool includeInside = false) const;
    bool isPointInsideTile(const Camera &camera, Tile *tile, sf::Vector2f pointScreenPosition) const;
    sf::Vector2f getTileCornerScreenCoordinates(const Camera &camera, const TileCorner* corner) const;

    float getPointWeightInBrush(const sf::Vector2f& pointWorldPosition, const sf::Vector2f& brushCenterWorldPosition) const;

    void incrementBrushSize();
    void decrementBrushSize();
    std::string getBrushSizeValue() const;

    BrushView m_brushView;
    BrushMenu m_brushMenu;

    std::vector<TileCorner*> m_hoveredTileCorners;
    std::vector<Tile*> m_hoveredTiles;
    std::vector<Tile*> m_tilesToHilight;

    sf::Vector2i m_mouseWorldPosition;
    sf::Vector2f m_brushCenterWorldPosition;
    std::vector<BrushTileCornerHit> m_brushSelectionTileCorners;
    std::vector<BrushTileCornerHit> m_brushSelectionTilesAsTileCorners;
    std::vector<BrushTileHit> m_brushSelectionTiles;

    int m_brushSize;
    int m_brushSizeMin;
    int m_brushSizeMax;

    std::vector<sf::Image> m_brushesImages;
    int m_currentBrushImage;
};

#endif //LANDCRAFT_BRUSHCONTROLLER_HPP
