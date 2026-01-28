#ifndef TILE_CORNER_HPP
#define TILE_CORNER_HPP

#include <SFML/Graphics.hpp>

enum class SelectionMode {
    TILE,
    TILE_CORNER
};

struct TileCorner {
    sf::Vector2i Position;
    int Height;
    sf::Color Color;
    // tiletype
};

struct ScreenTileCorner {
    sf::Vector2f ScreenPosition;
    sf::Vector2i WorldPosition;
    int Height;
    sf::Color Color;
    // tiletype
};

#endif // TILE_CORNER_HPP