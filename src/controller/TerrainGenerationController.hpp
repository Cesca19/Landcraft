//
// Created by fran on 02/06/2026.
//

#ifndef LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP
#define LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP

#include <ctime>
#include <vector>
#include <iostream>
#include "../../lib/FastNoiseLite.h"
#include <SFML/Graphics/Image.hpp>
#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"
#include "../commands/CommandHistory.hpp"
#include "../commands/SetSplatMapCommand.hpp"
#include "../commands/CommandGroup.hpp"
#include "../commands/SetTerrainHeightMapCommand.hpp"
#include "../view/menu/TerrainGenerationMenu.hpp"

class TerrainGenerationController {
public:
    TerrainGenerationController(const sf::Vector2f &terrainGenerationMenuPosition, const sf::Vector2u &windowSize);
    float getNoise(float nx, float ny);
    std::vector<std::vector<float>> generateHeightmap(int width, int height);
    std::vector<std::vector<float>> generateTerrainHeightmap(WorldModel &model, WorldView &view);
    void generateTerrain(WorldModel &model, WorldView &view, CommandHistory &commandHistory);
    void handleEvents(const sf::Event &event, sf::RenderWindow& window, 
        WorldModel &model, WorldView &view, CommandHistory &commandHistory);
private:
    FastNoiseLite m_noise;
    FastNoiseLite::NoiseType m_currentNoiseType;
    std::unique_ptr<TerrainGenerationMenu> m_terrainGenerationMenu;
};


#endif //LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP