//
// Created by fran on 02/06/2026.
//

#include "TerrainGenerationController.hpp"

TerrainGenerationController::TerrainGenerationController(const sf::Vector2f &terrainGenerationMenuPosition, const sf::Vector2u &windowSize)
    : m_currentNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2)
{
    m_noise.SetNoiseType(m_currentNoiseType);
    m_noise.SetFrequency(1);
    m_terrainGenerationMenu = std::make_unique<TerrainGenerationMenu>(terrainGenerationMenuPosition, windowSize);
    
    m_terrainGenerationMenu->initOnTerrainGenerationMenuButtonClickCallback([this]() {
        m_terrainGenerationMenu->setTerrainGenerationMenuVisibility(true);
    });
    m_terrainGenerationMenu->initOnCloseTerrainGenerationMenuButtonClickCallback([this]() {
        m_terrainGenerationMenu->setTerrainGenerationMenuVisibility(false);
    });
    m_terrainGenerationMenu->initOnGenerateButtonClickCallback([this]() {
    });
}

float TerrainGenerationController::getNoise(float nx, float ny)
{
    return m_noise.GetNoise(nx, ny);
}

std::vector<std::vector<float>> TerrainGenerationController::generateHeightmap(int width, int height)
{
    std::vector<std::vector<float>> heightmap(height, std::vector<float>(width));
    
    float baseNoiseFrequency = 1.0f;
    // -> fastNoise is deterministic, so we can use the same seed for the same heightmap size to get the same result
    // If we want to get a different heightmap each time, we can use a random seed, for example based on the current time
    int noiseSeed = 1337;
    std::cout << "Generating heightmap with seed: " << std::time(nullptr) << std::endl;
    m_noise.SetSeed(noiseSeed);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // -> first we want to scale the coordinates to the range [0, 1], 
            // so we divide them by the width and height of the heightmap
            // -> then we want to set the center of the heightmap at (0, 0) in noise space, 
            // so we need to offset the coordinates by half the width and height
            float nx = (static_cast<float>(x) / static_cast<float>(width)) - 0.5f;
            float ny = (static_cast<float>(y) / static_cast<float>(height)) - 0.5f;
            float elevation = 0.0f;

            // -> An octave is a layer of noise with a specific frequency and amplitude. 
            // We can use this formula: amplitude * getNoise(nx * frequency, ny * frequency) which is an octave of noise
            // like this heightmap[y][x] = getNoise(nx * baseNoiseFrequency, ny * baseNoiseFrequency);
            // -> The frequency indate how much the noise changes between two points. 
            // A higher frequency will result in a more detailed heightmap, while a lower frequency will result in a smoother heightmap.
            // -> But with just one octave, the heightmap will look very simple, so we can add multiple octaves together 
            // to get a more complex heightmap, The higher the amplitude, the more influence that octave will have on the final heightmap.
            // we will change the seed for each octave to get a different noise pattern
            elevation = 1.0f * getNoise(nx * baseNoiseFrequency, ny * baseNoiseFrequency)
                    + 0.5f * getNoise(nx * baseNoiseFrequency * 2, ny * baseNoiseFrequency * 2)
                    + 0.25f * getNoise(nx * baseNoiseFrequency * 4, ny * baseNoiseFrequency * 4);
            elevation /= (1.0f + 0.5f + 0.25f);
            heightmap[y][x] = elevation;
            // Like this The continents will be defined by the first octave, 
            // the second octave will add some hills and valleys, and the third octave will add some small details to the heightmap.
        }
    }
    return heightmap;
}

std::vector<std::vector<float>> TerrainGenerationController::generateTerrainHeightmap(WorldModel &model, WorldView &view)
{
    std::vector<std::vector<std::unique_ptr<TileCorner>>>& corners = model.getCorners();
    int width = corners[0].size();
    int height = corners.size();
    int minElevation = model.getMinElevation();
    int maxElevation = model.getMaxElevation();
    float noiseMin = -1.0f;
    float noiseMax = 1.0f;
    std::vector<std::vector<float>> heightmap = generateHeightmap(width, height);
    std::vector<std::vector<float>> finalHeightmap(height, std::vector<float>(width));

    // TO DO: turn that into a custom command that we can undo/redo 
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float heightValue = heightmap[y][x];
            // scale height to [0, 1]
            float normalizedHeight = (heightValue - noiseMin) / (noiseMax - noiseMin);
            // we want to scale the height value to the range [minElevation, maxElevation]
            float scaledHeight = normalizedHeight * (maxElevation - minElevation) + minElevation; // scale to [minElevation, maxElevation]
            // corners[y][x]->setHeight(scaledHeight);
            finalHeightmap[y][x] = scaledHeight;
        }
    }
    return finalHeightmap;
}

void TerrainGenerationController::generateTerrain(WorldModel &model, WorldView &view, CommandHistory &commandHistory)
{
    std::vector<std::vector<float>> heightmap = generateTerrainHeightmap(model, view);
    sf::Image blankImage;
    sf::Image splatmapImage = view.getSplatmapImage();
    blankImage.create(splatmapImage.getSize().x, splatmapImage.getSize().y, sf::Color(255, 0, 0, 0));
    std::unique_ptr<SetTerrainHeightMapCommand> heightmapCommand = std::make_unique<SetTerrainHeightMapCommand>(heightmap);
    std::unique_ptr<SetSplatMapCommand> splatmapCommand = std::make_unique<SetSplatMapCommand>(view.getSplatmapImage(), blankImage);
    std::unique_ptr<CommandGroup> commandGroup = std::make_unique<CommandGroup>("Generate Terrain");
    commandGroup->addCommand(std::move(heightmapCommand));
    commandGroup->addCommand(std::move(splatmapCommand));
    
    commandHistory.addCommand(std::move(commandGroup), model, view, true);
}

void TerrainGenerationController::handleEvents(const sf::Event &event, sf::RenderWindow &window, 
    WorldModel &model, WorldView &view, CommandHistory &commandHistory)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
        std::cout << "Generating terrain..." << std::endl;
        generateTerrain(model, view, commandHistory);
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
        m_currentNoiseType = (m_currentNoiseType == FastNoiseLite::NoiseType::NoiseType_Perlin) 
            ? FastNoiseLite::NoiseType::NoiseType_OpenSimplex2 
            : FastNoiseLite::NoiseType::NoiseType_Perlin;
        m_noise.SetNoiseType(m_currentNoiseType);
        std::cout << "Noise type changed to: " << (m_currentNoiseType == FastNoiseLite::NoiseType::NoiseType_Perlin ? "Perlin" : "OpenSimplex2") << std::endl;
    }
}
