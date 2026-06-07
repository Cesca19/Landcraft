//
// Created by fran on 02/06/2026.
//

#include "TerrainGenerationController.hpp"

TerrainGenerationController::TerrainGenerationController(const sf::Vector2f &terrainGenerationMenuPosition, 
    const sf::Vector2u &windowSize, WorldModel *model, WorldView *view, CommandHistory *commandHistory)
    : m_seed(std::time(nullptr))
    , m_frequency(1.0f)
    , m_octaves(3)
    , m_exponent(1.0f)
    , m_frequencyIncreaseStep(0.1f)
    , m_octavesIncreaseStep(1)
    , m_exponentIncreaseStep(0.1f)
    , m_isHeightStepModeEnabled(false)
    , m_stepsNb(1)
    , m_model(model)
    , m_view(view)
    , m_commandHistory(commandHistory)
    , m_currentNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2)
{
    m_noise.SetNoiseType(m_currentNoiseType);
    m_noise.SetFrequency(1);
    m_noiseTypeNames = {
        "OpenSimplex2 ",
        "OpenSimplex2S",
        "  Cellular   ",
        "    Perlin   ",
        "  ValueCubic ",
        "  WhiteNoise "
    };
    m_terrainGenerationMenu = std::make_unique<TerrainGenerationMenu>(terrainGenerationMenuPosition, windowSize);
    m_terrainGenerationMenu->setFrequencyValueText(MathUtils::toString(m_frequency));
    m_terrainGenerationMenu->setOctavesValueText(std::to_string(m_octaves));
    m_terrainGenerationMenu->setExponentValueText(MathUtils::toString(m_exponent));
    m_terrainGenerationMenu->selectNoiseType(m_noiseTypeNames[static_cast<int>(m_currentNoiseType)]);
    m_terrainGenerationMenu->setHeightStepMode(m_isHeightStepModeEnabled);
    m_terrainGenerationMenu->setStepsNbValueText(std::to_string(m_stepsNb));
    
    m_terrainGenerationMenu->initOnTerrainGenerationMenuButtonClickCallback([this]() {
        m_terrainGenerationMenu->setTerrainGenerationMenuVisibility(true);
    });
    m_terrainGenerationMenu->initOnCloseTerrainGenerationMenuButtonClickCallback([this]() {
        m_terrainGenerationMenu->setTerrainGenerationMenuVisibility(false);
    });
    m_terrainGenerationMenu->initOnGenerateButtonClickCallback([this]() {
        onGenerateButtonClick();
    });
    m_terrainGenerationMenu->initOnSeedInputValidatedCallback([this](const std::string &seed) {
        onSeedInputValidated(seed);
    });
    m_terrainGenerationMenu->initOnRandomSeedButtonClickCallback([this]() {
        onRandomSeedButtonClick();
    });
    m_terrainGenerationMenu->initOnFrequencyIncreaseButtonClickCallback([this]() {
        addFrequencyStep(1);
    });
    m_terrainGenerationMenu->initOnFrequencyDecreaseButtonClickCallback([this]() {
        addFrequencyStep(-1);
    });
    m_terrainGenerationMenu->initOnOctavesIncreaseButtonClickCallback([this]() {
        addOctavesStep(1);
    });
    m_terrainGenerationMenu->initOnOctavesDecreaseButtonClickCallback([this]() {
        addOctavesStep(-1);
    });
    m_terrainGenerationMenu->initOnExponentIncreaseButtonClickCallback([this]() {
        addExponentStep(1);
    });
    m_terrainGenerationMenu->initOnExponentDecreaseButtonClickCallback([this]() {
        addExponentStep(-1);
    });
    m_terrainGenerationMenu->initOnNextNoiseTypeButtonClickCallback([this]() {
        addNoiseTypeStep(1);
    });
    m_terrainGenerationMenu->initOnPreviousNoiseTypeButtonClickCallback([this]() {
        addNoiseTypeStep(-1);
    });
    m_terrainGenerationMenu->initOnHeightStepOnButtonClickCallback([this]() {
        setHeightStepMode(true);
    });
    m_terrainGenerationMenu->initOnHeightStepOffButtonClickCallback([this]() {
        setHeightStepMode(false);
    });
    m_terrainGenerationMenu->initOnStepsNbIncreaseButtonClickCallback([this]() {
        addStepsNb(1);
    });
    m_terrainGenerationMenu->initOnStepsNbDecreaseButtonClickCallback([this]() {
        addStepsNb(-1);
    });
}

float TerrainGenerationController::getNoise(const float nx, const float ny) const
{
    return m_noise.GetNoise(nx, ny);
}

std::vector<std::vector<float>> TerrainGenerationController::generateHeightmap(const int width, const int height)
{
    std::vector<std::vector<float>> heightmap(height, std::vector<float>(width));
    
    // -> fastNoise is deterministic, so we can use the same seed for the same heightmap size to get the same result
    // If we want to get a different heightmap each time, we can use a random seed, for example based on the current time
    m_noise.SetSeed(m_seed);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // -> first we want to scale the coordinates to the range [0, 1], 
            // so we divide them by the width and height of the heightmap
            // -> then we want to set the center of the heightmap at (0, 0) in noise space, 
            // so we need to offset the coordinates by half the width and height
            const float nx = (static_cast<float>(x) / static_cast<float>(width)) - 0.5f;
            const float ny = (static_cast<float>(y) / static_cast<float>(height)) - 0.5f;
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
    const std::vector<std::vector<std::unique_ptr<TileCorner>>>& corners = m_model->getCorners();
    const int width = corners[0].size();
    const int height = corners.size();
    const int minElevation = m_model->getMinElevation();
    const int maxElevation = m_model->getMaxElevation();
    const std::vector<std::vector<float>> heightmap = generateHeightmap(width, height);
    std::vector<std::vector<float>> finalHeightmap(height, std::vector<float>(width));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            constexpr float noiseMax = 1.0f;
            constexpr float noiseMin = -1.0f;
            const float heightValue = heightmap[y][x];
            // scale height to [0, 1]
            float normalizedHeight = (heightValue - noiseMin) / (noiseMax - noiseMin);
            normalizedHeight = applyHeightStep(normalizedHeight);
            // we want to scale the height value to the range [minElevation, maxElevation]
            const float scaledHeight = normalizedHeight * (maxElevation - minElevation) + minElevation; // scale to [minElevation, maxElevation]
            finalHeightmap[y][x] = scaledHeight;
        }
    }
    return finalHeightmap;
}

void TerrainGenerationController::generateTerrain()
{
    std::vector<std::vector<float>> heightmap = generateTerrainHeightmap();
    sf::Image blankImage;
    sf::Image splatMapImage = m_view->getSplatmapImage();
    blankImage.create(splatMapImage.getSize().x, splatMapImage.getSize().y, sf::Color(255, 0, 0, 0));
    std::unique_ptr<SetTerrainHeightMapCommand> heightmapCommand = std::make_unique<SetTerrainHeightMapCommand>(heightmap);
    std::unique_ptr<SetSplatMapCommand> splatMapCommand = std::make_unique<SetSplatMapCommand>(m_view->getSplatmapImage(), blankImage);
    std::unique_ptr<CommandGroup> commandGroup = std::make_unique<CommandGroup>("Generating Terrain");
    commandGroup->addCommand(std::move(heightmapCommand));
    commandGroup->addCommand(std::move(splatMapCommand));
    
    m_commandHistory->addCommand(std::move(commandGroup), *m_model, *m_view, true);
    // m_terrainGenerationMenu->setTerrainGenerationMenuVisibility(false);
}

void TerrainGenerationController::handleEvents(const sf::Event &event, sf::RenderWindow &window)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G)
        onGenerateButtonClick();
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N)
        addNoiseTypeStep(1);
}

float TerrainGenerationController::applyHeightStep(const float height) const
{
    if (!m_isHeightStepModeEnabled || m_stepsNb < 1)
        return height;
    return std::round(height * m_stepsNb) / m_stepsNb;
}

void TerrainGenerationController::onGenerateButtonClick()
{
    const std::string seedText = m_terrainGenerationMenu->getSeedInputText();

    if (!seedText.empty())
        m_seed = std::stoi(seedText);
    else
        onRandomSeedButtonClick();
    generateTerrain();
}

void TerrainGenerationController::onSeedInputValidated(const std::string &seed)
{
    m_seed = std::stoi(seed);
}

void TerrainGenerationController::onRandomSeedButtonClick()
{
    m_seed = static_cast<int>(std::time(nullptr) ^ std::rand());
    m_terrainGenerationMenu->setSeedInputText(std::to_string(m_seed));
}

void TerrainGenerationController::addFrequencyStep(const int factor)
{
    m_frequency += m_frequencyIncreaseStep * factor;
    m_frequency = std::clamp(m_frequency, 1.0f, 10.0f);
    m_terrainGenerationMenu->setFrequencyValueText(MathUtils::toString(m_frequency));
}

void TerrainGenerationController::addOctavesStep(const int factor)
{
    m_octaves += m_octavesIncreaseStep * factor;
    m_octaves = std::clamp(m_octaves, 1, 10);
    m_terrainGenerationMenu->setOctavesValueText(std::to_string(m_octaves));
}

void TerrainGenerationController::addExponentStep(const int factor)
{
    m_exponent += m_exponentIncreaseStep * factor;
    m_exponent = std::clamp(m_exponent, 0.01f, 10.0f);
    m_terrainGenerationMenu->setExponentValueText(MathUtils::toString(m_exponent));
}

void TerrainGenerationController::addNoiseTypeStep(const int factor)
{
    const int noiseTypeCount = m_noiseTypeNames.size();
    int currentNoiseTypeIndex = static_cast<int>(m_currentNoiseType);
    currentNoiseTypeIndex = (currentNoiseTypeIndex + factor + noiseTypeCount) % noiseTypeCount;
    m_currentNoiseType = static_cast<FastNoiseLite::NoiseType>(currentNoiseTypeIndex);
    m_noise.SetNoiseType(m_currentNoiseType);
    m_terrainGenerationMenu->selectNoiseType(m_noiseTypeNames[currentNoiseTypeIndex]);
}

void TerrainGenerationController::setHeightStepMode(const bool isOn)
{
    m_isHeightStepModeEnabled = isOn;
    m_terrainGenerationMenu->setHeightStepMode(isOn);
}

void TerrainGenerationController::addStepsNb(const int factor)
{
    if (!m_isHeightStepModeEnabled)
        return;
    m_stepsNb += factor;
    m_stepsNb = std::clamp(m_stepsNb, 1, 20);
    m_terrainGenerationMenu->setStepsNbValueText(std::to_string(m_stepsNb));
}
