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

enum class SelectionMode {
    TILE,
    TILE_CORNER
};

class SelectionController {
public:
    SelectionController();
    ~SelectionController();
    void update(float deltaTime, sf::RenderWindow &window, SelectionMode selectionMode,
                 WorldModel &worldModel, const Camera &camera, bool &hasModelChanged);
    void draw(sf::RenderWindow &window, const Camera &camera);

    std::vector<TileCorner *> getSelectedTileCorners() const;
    const std::vector<Tile *> &getSelectedTiles() const;
    sf::Vector2i getMouseWorldPosition() const;
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

    SelectionView m_selectionView;
    std::vector<TileCorner *> m_selectedTileCorners;
    std::vector<Tile *> m_selectedTiles;
    sf::Vector2i m_mouseWorldPosition;
};

#endif //LANDCRAFT_SELECTIONCONTROLLER_HPP
