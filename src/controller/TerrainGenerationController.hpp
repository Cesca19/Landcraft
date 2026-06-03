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
#include "../commands/CommandHistory.hpp"
#include "../commands/GenerateTerrainCommand.hpp"

class TerrainGenerationController {
public:
    TerrainGenerationController();
    float getNoise(float nx, float ny);
    std::vector<std::vector<float>> generateHeightmap(int width, int height);
    std::vector<std::vector<float>> generateTerrainHeightmap(WorldModel &model, WorldView &view);
    void generateTerrain(WorldModel &model, WorldView &view, CommandHistory &commandHistory);
    void handleEvents(const sf::Event &event, sf::RenderWindow& window, 
        WorldModel &model, WorldView &view, CommandHistory &commandHistory);
private:
    FastNoiseLite m_noise;
    FastNoiseLite::NoiseType m_currentNoiseType;
};


#endif //LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP