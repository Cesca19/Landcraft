//
// Created by fran on 02/06/2026.
//

#include "TerrainGenerationController.hpp"

TerrainGenerationController::TerrainGenerationController(const sf::Vector2f &terrainGenerationMenuPosition, 
    const sf::Vector2u &windowSize, WorldModel *model, WorldView *view, CommandHistory *commandHistory)
    : m_currentNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2)
    , m_seed(std::time(nullptr))
    , m_frequency(1.0f)
    , m_octaves(3)
    , m_exponent(1.0f)
    , m_frequencyIncreaseStep(0.1f)
    , m_octavesIncreaseStep(1)
    , m_exponentIncreaseStep(0.1f)
    , m_model(model)
    , m_view(view)
    , m_commandHistory(commandHistory)
{
    m_noise.SetNoiseType(m_currentNoiseType);
    m_noise.SetFrequency(1);
    m_terrainGenerationMenu = std::make_unique<TerrainGenerationMenu>(terrainGenerationMenuPosition, windowSize);
    m_terrainGenerationMenu->setFrequencyValueText(MathUtils::toString(m_frequency));
    m_terrainGenerationMenu->setOctavesValueText(std::to_string(m_octaves));
    m_terrainGenerationMenu->setExponentValueText(MathUtils::toString(m_exponent));
    
    m_terrainGenerationMenu->initOnTerrainGenerationMenuButtonClickCallback([this]() {
        m_terrainGenerationMenu->setTerrainGenerationMenuVisibility(true);
    });
    m_terrainGenerationMenu->initOnCloseTerrainGenerationMenuButtonClickCallback([this]() {
        m_terrainGenerationMenu->setTerrainGenerationMenuVisibility(false);
    });
    m_terrainGenerationMenu->initOnGenerateButtonClickCallback([this]() {
        OnGenerateButtonClick();
    });
    m_terrainGenerationMenu->initOnSeedInputValidatedCallback([this](const std::string &seed) {
        OnSeedInputValidated(seed);
    });
    m_terrainGenerationMenu->initOnRandomSeedButtonClickCallback([this]() {
        OnRandomSeedButtonClick();
    });
    m_terrainGenerationMenu->initOnFrequencyIncreaseButtonClickCallback([this]() {
        AddFrequencyStep(1);
    });
    m_terrainGenerationMenu->initOnFrequencyDecreaseButtonClickCallback([this]() {
        AddFrequencyStep(-1);
    });
    m_terrainGenerationMenu->initOnOctavesIncreaseButtonClickCallback([this]() {
        AddOctavesStep(1);
    });
    m_terrainGenerationMenu->initOnOctavesDecreaseButtonClickCallback([this]() {
        AddOctavesStep(-1);
    });
    m_terrainGenerationMenu->initOnExponentIncreaseButtonClickCallback([this]() {
        AddExponentStep(1);
    });
    m_terrainGenerationMenu->initOnExponentDecreaseButtonClickCallback([this]() {
        AddExponentStep(-1);
    });
}

float TerrainGenerationController::getNoise(float nx, float ny)
{
    return m_noise.GetNoise(nx, ny);
}

std::vector<std::vector<float>> TerrainGenerationController::generateHeightmap(int width, int height)
{
    std::vector<std::vector<float>> heightmap(height, std::vector<float>(width));
    
    // -> fastNoise is deterministic, so we can use the same seed for the same heightmap size to get the same result
    // If we want to get a different heightmap each time, we can use a random seed, for example based on the current time
    std::cout << "Generating heightmap with seed: " << m_seed << std::endl;
    m_noise.SetSeed(m_seed);

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
            float frequency = m_frequency;
            float amplitude = 1.0f;
            float amplitudeSum = 0.0f;
            for (int octave = 0; octave < m_octaves; octave++) {
                elevation += amplitude * getNoise(nx * frequency, ny * frequency);
                amplitudeSum += amplitude;
                frequency *= 2.0f; // increase frequency for the next octave
                amplitude *= 0.5f; // decrease amplitude for the next octave
            }
            elevation /= amplitudeSum;
            // -> apply exponent to change the distribution of height values,
            // higher exponent will result in more flat areas and sharper peaks
            float sign = (elevation >= 0) ? 1.0f : -1.0f;
            heightmap[y][x] = sign * std::pow(std::abs(elevation), m_exponent);
            // Like this The continents will be defined by the first octave, 
            // the second octave will add some hills and valleys, and the third octave will add some small details to the heightmap.
        }
    }
    return heightmap;
}

std::vector<std::vector<float>> TerrainGenerationController::generateTerrainHeightmap()
{
    std::vector<std::vector<std::unique_ptr<TileCorner>>>& corners = m_model->getCorners();
    int width = corners[0].size();
    int height = corners.size();
    int minElevation = m_model->getMinElevation();
    int maxElevation = m_model->getMaxElevation();
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

void TerrainGenerationController::generateTerrain()
{
    std::vector<std::vector<float>> heightmap = generateTerrainHeightmap();
    sf::Image blankImage;
    sf::Image splatmapImage = m_view->getSplatmapImage();
    blankImage.create(splatmapImage.getSize().x, splatmapImage.getSize().y, sf::Color(255, 0, 0, 0));
    std::unique_ptr<SetTerrainHeightMapCommand> heightmapCommand = std::make_unique<SetTerrainHeightMapCommand>(heightmap);
    std::unique_ptr<SetSplatMapCommand> splatmapCommand = std::make_unique<SetSplatMapCommand>(m_view->getSplatmapImage(), blankImage);
    std::unique_ptr<CommandGroup> commandGroup = std::make_unique<CommandGroup>("Generate Terrain");
    commandGroup->addCommand(std::move(heightmapCommand));
    commandGroup->addCommand(std::move(splatmapCommand));
    
    m_commandHistory->addCommand(std::move(commandGroup), *m_model, *m_view, true);
}

void TerrainGenerationController::handleEvents(const sf::Event &event, sf::RenderWindow &window)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
        std::cout << "Generating terrain..." << std::endl;
        generateTerrain();
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
        m_currentNoiseType = (m_currentNoiseType == FastNoiseLite::NoiseType::NoiseType_Perlin) 
            ? FastNoiseLite::NoiseType::NoiseType_OpenSimplex2 
            : FastNoiseLite::NoiseType::NoiseType_Perlin;
        m_noise.SetNoiseType(m_currentNoiseType);
        std::cout << "Noise type changed to: " << (m_currentNoiseType == FastNoiseLite::NoiseType::NoiseType_Perlin ? "Perlin" : "OpenSimplex2") << std::endl;
    }
}

void TerrainGenerationController::OnGenerateButtonClick()
{
    std::cout << "Generating terrain..." << std::endl;
    std::string seedText = m_terrainGenerationMenu->getSeedInputText();

    if (!seedText.empty())
        m_seed = std::stoi(seedText);
    else
        OnRandomSeedButtonClick();
    generateTerrain();
}

void TerrainGenerationController::OnSeedInputValidated(const std::string &seed)
{
    m_seed = std::stoi(seed);
}

void TerrainGenerationController::OnRandomSeedButtonClick()
{
    m_seed = static_cast<int>(std::time(nullptr) ^ std::rand());
    m_terrainGenerationMenu->setSeedInputText(std::to_string(m_seed));
}

void TerrainGenerationController::AddFrequencyStep(int factor)
{
    m_frequency += m_frequencyIncreaseStep * factor;
    m_frequency = std::clamp(m_frequency, 1.0f, 10.0f);
    m_terrainGenerationMenu->setFrequencyValueText(MathUtils::toString(m_frequency));
}

void TerrainGenerationController::AddOctavesStep(int factor)
{
    m_octaves += m_octavesIncreaseStep * factor;
    m_octaves = std::clamp(m_octaves, 1, 10);
    m_terrainGenerationMenu->setOctavesValueText(std::to_string(m_octaves));
}

void TerrainGenerationController::AddExponentStep(int factor)
{
    m_exponent += m_exponentIncreaseStep * factor;
    m_exponent = std::clamp(m_exponent, 0.01f, 10.0f);
    m_terrainGenerationMenu->setExponentValueText(MathUtils::toString(m_exponent));
}
