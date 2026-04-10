//
// Created by fran on 02/04/2026.
//

#include "LandcraftEditor.hpp"

LandcraftEditor::LandcraftEditor()
    : m_hasFocus(true)
    , m_windowSize(sf::Vector2f(1200, 800))
    , m_viewSize(m_windowSize)
    , m_tileSizeX(64)
    , m_tileSizeY(64)
    , m_heightScale(6) // => 64 / 8
    , m_projectionAngleX(30)
    , m_projectionAngleY(15) // 35.264 realistic isometric angle
    , m_window(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Landcraft")
    , m_worldController(nullptr)
    , m_uiController(nullptr)
{
    m_uiController = std::make_unique<UIController>();
    UIFactory::init(m_uiController.get());
    m_worldController = std::make_unique<WorldController>();

	m_window.setVerticalSyncEnabled(true);
    m_window.setIcon(512, 512, ResourceManager::getInstance()
            .getImage("assets/textures/ui/landcraft_icon_512.png").getPixelsPtr());

    m_worldController->init("assets/maps/map.txt",
        {m_tileSizeX, m_tileSizeY, m_heightScale, m_projectionAngleX, m_projectionAngleY},
        {sf::Vector2f{0, 0}, sf::Vector2f{1200, 800}, m_windowSize});
}

void LandcraftEditor::run()
{
    m_clock.restart();
    float deltaTime = 0;
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
            || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            m_window.close();
        switch (event.type) {
            case sf::Event::LostFocus:
                m_hasFocus = false;
                // stop continuous events
                break;
            case sf::Event::GainedFocus:
                m_hasFocus = true;
				m_clock.restart();
                break;
            case sf::Event::Resized:
                m_worldController->onWindowResized(sf::Vector2u(event.size.width, event.size.height));
                break;
            default:
                break;
        }
        if (m_hasFocus) {
            m_uiController->handleEvents(event, m_window);
            m_worldController->handleEvents(event, m_window);
        }
    }
}

void LandcraftEditor::handleContinuousEvents(const float deltaTime)
{
    m_uiController->handleContinuousEvents(deltaTime, m_window);
	m_worldController->handleContinuousEvents(deltaTime, m_window);
}