//
// Created by fran on 02/06/2026.
//

#ifndef LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP
#define LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP

#include <cmath>
#include <ctime>
#include <vector>
#include <iostream>
#include "../../lib/FastNoiseLite.h"
#include <SFML/Graphics/Image.hpp>
#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"
#include "../utils/MathUtils.hpp"
#include "../commands/CommandHistory.hpp"
#include "../commands/SetSplatMapCommand.hpp"
#include "../commands/CommandGroup.hpp"
#include "../commands/SetTerrainHeightMapCommand.hpp"
#include "../view/menu/TerrainGenerationMenu.hpp"

class TerrainGenerationController {
public:
    TerrainGenerationController(const sf::Vector2f &terrainGenerationMenuPosition, const sf::Vector2u &windowSize,
        WorldModel *model, WorldView *view, CommandHistory *commandHistory);
    float getNoise(float nx, float ny);
    std::vector<std::vector<float>> generateHeightmap(int width, int height);
    std::vector<std::vector<float>> generateTerrainHeightmap();
    void generateTerrain();
    void handleEvents(const sf::Event &event, sf::RenderWindow& window);
private:
    void OnGenerateButtonClick();
    void OnSeedInputValidated(const std::string &seed);
    void OnRandomSeedButtonClick();
    void AddFrequencyStep(int factor);
    void AddOctavesStep(int factor);
    void AddExponentStep(int factor);

    int m_seed;
    float m_frequency;
    int m_octaves;
    float m_exponent;
    float m_frequencyIncreaseStep;
    int m_octavesIncreaseStep;
    float m_exponentIncreaseStep;
    WorldModel *m_model;
    WorldView *m_view;
    CommandHistory *m_commandHistory;
    FastNoiseLite m_noise;
    FastNoiseLite::NoiseType m_currentNoiseType;
    std::unique_ptr<TerrainGenerationMenu> m_terrainGenerationMenu;
};


#endif //LANDCRAFT_TERRAINGENERATIONCONTROLLER_HPP