#ifndef TILE_CORNER_HPP
#define TILE_CORNER_HPP

#include <SFML/Graphics.hpp>

enum class SelectionMode {
    TILE,
    TILE_CORNER
};

struct TileCorner {
    sf::Vector2i Position;
    float Height;
    sf::Color Color;
    // tiletype
};

struct ScreenTileCorner {
    ScreenTileCorner(const sf::Vector2f screenPosition, const sf::Vector2i worldPosition,
                    const float screenHeight, const float worldHeight, const sf::Color color) {
        ScreenPosition = screenPosition;
        WorldPosition = worldPosition;
        ScreenHeight = screenHeight;
        WorldHeight = worldHeight;
        Color = color;
        RotatedWorldPosition = sf::Vector2f(worldPosition);
    }
    sf::Vector2f ScreenPosition;
    sf::Vector2i WorldPosition;
    float ScreenHeight;
    float WorldHeight;
    sf::Color Color;
    sf::Vector2f RotatedWorldPosition;
    // tiletype
};

#endif // TILE_CORNER_HPP