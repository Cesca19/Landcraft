//
// Created by fran on 15/05/2026.
//

#ifndef LANDCRAFT_PAINTSTROKE_HPP
#define LANDCRAFT_PAINTSTROKE_HPP

#include <SFML/System/Vector2.hpp>

struct PaintStroke {
    sf::Vector2f worldPosition; // Impact position in world grid coordinates (e.g., X: 10.5, Y: 20.2)
    int brushTextureId;         // ID of the brush shape (e.g., 5 for Star shape)
    int textureId;             // Painting Texture ID (0=Clear, 1=Grass, 2=Sand, etc.)
    float radius;               // Radius of the brush in grid units (tiles)
};

#endif //LANDCRAFT_PAINTSTROKE_HPP