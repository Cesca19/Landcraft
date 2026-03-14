//
// Created by fattindehou on 11/03/2026.
//

#ifndef LANDCRAFT_TILECORNER_HPP
#define LANDCRAFT_TILECORNER_HPP

#include <string>
#include <SFML/System/Vector2.hpp>

class TileCorner {
public:
    TileCorner(int row, int col, int height, int textureID);
    float getRow() const;
    float getColumn() const;
    float getHeight() const;
    int getTextureID() const;
    sf::Vector2f getPosition() const;
    void addHeight(int heightStep);
private:    
    int m_row;
    int m_col;
    int m_height;
    int m_textureID;
};


#endif //LANDCRAFT_TILECORNER_HPP