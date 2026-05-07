//
// Created by fran on 30/04/2026.
//

#include "EditorLoadingController.hpp"

bool EditorLoadingController::initializeWorld(sf::RenderWindow& window, const sf::Vector2u& windowSize,
    std::string mapName, WorldController& worldController, const CameraSettings& cameraSettings,
    const ViewSettings& viewSettings) const
{
    // if (!drawLoadingScreen(window, windowSize, "Loading editor...")) return false;
    if (!drawLoadingScreen(window, windowSize, "Loading world...")) return false;

    worldController.init(mapName, cameraSettings, viewSettings);
    return true;
}

bool EditorLoadingController::drawLoadingScreen(sf::RenderWindow& window, const sf::Vector2u& windowSize, const std::string& message) const
{
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return false;
        }
    }

    window.clear(sf::Color(235, 230, 250));

    sf::Text loadingText;
    loadingText.setFont(ResourceManager::getInstance().getFont("assets/fonts/ComicRelief-Regular.ttf"));
    loadingText.setString(message);
    loadingText.setCharacterSize(32);
    loadingText.setStyle(sf::Text::Bold);
    loadingText.setFillColor(sf::Color(90, 70, 130));

    const sf::FloatRect bounds = loadingText.getLocalBounds();
    loadingText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    loadingText.setPosition(static_cast<float>(windowSize.x) / 2.0f, static_cast<float>(windowSize.y) / 2.0f);

    window.draw(loadingText);
    window.display();
    return true;
}
