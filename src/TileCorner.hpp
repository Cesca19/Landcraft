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
    ScreenTileCorner(const sf::Vector2f screenPosition, const sf::Vector2i worldPosition,
                    const int screenHeight, const int worldHeight, const sf::Color color) {
        ScreenPosition = screenPosition;
        WorldPosition = worldPosition;
        ScreenHeight = screenHeight;
        WorldHeight = worldHeight;
        Color = color;
        RotatedWorldPosition = sf::Vector2f(worldPosition);
    }
    sf::Vector2f ScreenPosition;
    sf::Vector2i WorldPosition;
    int ScreenHeight;
    int WorldHeight;
    sf::Color Color;
    sf::Vector2f RotatedWorldPosition;
    // tiletype
};

#endif // TILE_CORNER_HPP