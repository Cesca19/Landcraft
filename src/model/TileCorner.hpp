//
// Created by fattindehou on 11/03/2026.
//

#ifndef LANDCRAFT_TILECORNER_HPP
#define LANDCRAFT_TILECORNER_HPP

#include <string>
#include <SFML/System/Vector2.hpp>

class TileCorner {
public:
    TileCorner(int row, int col, int height, 
        const sf::Vector2f& textureCoord, const std::string& textureName);
    
    int row;
    int col;
    int height;
    sf::Vector2f textureCoord;
    std::string textureName;
};


#endif //LANDCRAFT_TILECORNER_HPP