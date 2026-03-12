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
    int getRow() const;
    int getColumn() const;
    int getHeight() const;
    int getTextureID() const;
private:    
    int m_row;
    int m_col;
    int m_height;
    int m_textureID;
};


#endif //LANDCRAFT_TILECORNER_HPP