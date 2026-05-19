//
// Created by fattindehou on 11/03/2026.
//

#ifndef LANDCRAFT_TILECORNER_HPP
#define LANDCRAFT_TILECORNER_HPP

#include <string>
#include <algorithm>
#include <SFML/System/Vector2.hpp>

class TileCorner {
public:
    TileCorner(int row, int col, float height, float minElevation, float maxElevation);
    float getRow() const;
    float getColumn() const;
    float getHeight() const;
    sf::Vector2f getPosition() const;
    void addHeight(float heightStep);
private:    
    int m_row;
    int m_col;
    float m_height;

    float m_minElevation;
    float m_maxElevation;
};


#endif //LANDCRAFT_TILECORNER_HPP