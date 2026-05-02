//
// Created by fran on 02/04/2026.
//

#include "LandcraftEditor.hpp"

LandcraftEditor::LandcraftEditor()
    : m_hasFocus(true)
    , m_windowSize(sf::Vector2f(1920, 1080))
    , m_viewSize(m_windowSize)
    , m_tileSizeX(64)
    , m_tileSizeY(64)
    , m_heightScale(6) // => 64 / 8
    , m_projectionAngleX(30)
    , m_projectionAngleY(15) // 35.264 realistic isometric angle
    , m_window(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Landcraft", sf::Style::Default, sf::ContextSettings(0, 0, 4))
    , m_uiController(nullptr)
    , m_worldController(nullptr)
{
    // m_window.setPosition(sf::Vector2i(00, 600));
    m_uiController = std::make_unique<UIController>();
    m_uiController->setOnDestroy([] {
        UIFactory::init(nullptr);
    });
    UIFactory::init(m_uiController.get());
    m_worldController = std::make_unique<WorldController>();

    m_window.setVerticalSyncEnabled(true);
    applyWindowIcon();
}

void LandcraftEditor::run()
{
    const CameraSettings cameraSettings{
        m_tileSizeX, m_tileSizeY, m_heightScale, m_projectionAngleX, m_projectionAngleY
    };
    const ViewSettings viewSettings{
        sf::Vector2f{0, 0},
        sf::Vector2f{static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y)},
        m_windowSize
    };

    if (!m_appLoadingController.initializeWorld(m_window, m_windowSize, *m_worldController, cameraSettings, viewSettings)) {
        return;
    }

    m_clock.restart();
    float deltaTime = 0.0f;
    while (m_window.isOpen())
    {
        handleEvents();
        deltaTime = m_clock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        if (m_hasFocus) {
            handleContinuousEvents(deltaTime);
            m_uiController->update(deltaTime, m_window);
            m_worldController->update(deltaTime, m_window);
        }

        m_window.clear(sf::Color(196, 218, 242));
        m_worldController->draw(m_window);
        m_uiController->draw(m_window);
        m_window.display();
    }
}

void LandcraftEditor::handleEvents()
{
    sf::Event event;

    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed
            || (event.type == sf::Event::KeyPressed
                && event.key.code == sf::Keyboard::Escape
                && !m_uiController->isKeyBoardNavigatingHoverUI()))
            m_window.close();
        switch (event.type) {
            case sf::Event::LostFocus:
                m_hasFocus = false;
                // stop continuous events
                break;
            case sf::Event::GainedFocus:
                m_hasFocus = true;
                m_clock.restart();
                applyWindowIcon();
                break;
            case sf::Event::Resized:
                m_worldController->onWindowResized(sf::Vector2u(event.size.width, event.size.height));
                applyWindowIcon();
                break;
            default:
                break;
        }
        if (m_hasFocus) {
            m_uiController->handleEvents(event, m_window);

            if (m_uiController->shouldForwardEventToWorld(event))
                m_worldController->handleEvents(event, m_window);
        }
    }
}

void LandcraftEditor::handleContinuousEvents(const float deltaTime) const
{
    m_uiController->handleContinuousEvents(deltaTime, m_window);
    if (!m_uiController->isMouseHoverUI())
        m_worldController->handleContinuousEvents(deltaTime, m_window);
}

void LandcraftEditor::applyWindowIcon()
{
    const sf::Image &icon = ResourceManager::getInstance().getImage("assets/textures/ui/landcraft_icon_512.png");
    const sf::Vector2u iconSize = icon.getSize();
    if (iconSize.x == 0 || iconSize.y == 0)
        return;
    m_window.setIcon(iconSize.x, iconSize.y, icon.getPixelsPtr());
}
