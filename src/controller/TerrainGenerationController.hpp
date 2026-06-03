//
// Created by fran on 02/06/2026.
//

#ifndef LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP
#define LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP

#include <vector>
#include <iostream>
#include "../../lib/FastNoiseLite.h"

#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"

class TerrainGenerationController {
public:
    TerrainGenerationController();
    float getNoise(float nx, float ny);
    std::vector<std::vector<float>> generateHeightmap(int width, int height);
    void generateTerrain(WorldModel &model, WorldView &view);
    void handleEvents(const sf::Event &event, sf::RenderWindow& window, WorldModel &model, WorldView &view);
private:
    FastNoiseLite m_noise;
    FastNoiseLite::NoiseType m_currentNoiseType;
};


#endif //LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP