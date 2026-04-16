//
// Created by fran on 17/04/2026.
//

#ifndef LANDCRAFT_BRUSHINFOS_HPP
#define LANDCRAFT_BRUSHINFOS_HPP

#include "Tile.hpp"

enum class SelectionMode {
    TILE,
    TILE_CORNER
};

// here the weight is relative to the corner position
struct BrushTileCornerHit {
    TileCorner* corner;
    float weight;
};

// here the weight is relative to the tile center's position
struct BrushTileHit {
    Tile* tile;
    float weight;
};

#endif //LANDCRAFT_BRUSHINFOS_HPP