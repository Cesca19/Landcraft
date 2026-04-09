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
{
	m_window.setVerticalSyncEnabled(true);
    m_window.setIcon(512, 512, ResourceManager::getInstance()
            .getImage("assets/textures/ui/landcraft_icon_512.png").getPixelsPtr());

    m_worldController.init("assets/maps/map.txt",
        {m_tileSizeX, m_tileSizeY, m_heightScale, m_projectionAngleX, m_projectionAngleY},
        {sf::Vector2f{0, 0}, sf::Vector2f{1200, 800}, m_windowSize});
    UIFactory::init(&m_uiController);
}

void LandcraftEditor::run()
{
    // test ui
    const auto button = UIFactory::createTextButton({20, 20}, "Test Button", sf::Color::Black, 20);
    button->initStatesColors(sf::Color::Yellow, sf::Color::Blue, sf::Color::Cyan, sf::Color::Green);
    button->initOnClickCallback(  [] () {std::cout << "Button clicked" << std::endl; } );
    button->initBackgroundColor(sf::Color(95, 78, 121, 100));
    button->setVisibility(false);

    const auto iconButton = UIFactory::createSpriteButton("assets/textures/ui/brush_512.png", {400, 20}, {32, 32}, "Brush", 15);
    iconButton->initStatesColors(sf::Color(183, 183, 183), sf::Color(153, 153, 153), sf::Color::Cyan, sf::Color(163, 163, 163), sf::Color::Black);
    iconButton->initOnClickCallback([] () {std::cout << "icon Button clicked" << std::endl; } );
    iconButton->initHighlightTextAlign(HighlightTextAlign::Down);
    iconButton->initBackgroundStatesColor(sf::Color(255, 255, 255), sf::Color(250, 250, 250), sf::Color(250, 239, 250), sf::Color(246, 246, 246));


    m_clock.restart();
    float deltaTime = 0;
    while (m_window.isOpen())
    {
        handleEvents();
        deltaTime = m_clock.restart().asSeconds();
		if (deltaTime > 0.1f) deltaTime = 0.1f;

		if (m_hasFocus) {
			handleContinuousEvents(deltaTime);
		    m_uiController.update(deltaTime, m_window);
        	m_worldController.update(deltaTime, m_window);
		}

        m_window.clear(sf::Color(196, 218, 242));
        m_worldController.draw(m_window);
        m_uiController.draw(m_window);
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
                m_worldController.onWindowResized(sf::Vector2u(event.size.width, event.size.height));
                break;
            default:
                break;
        }
        if (m_hasFocus) {
            m_uiController.handleEvents(event, m_window);
            m_worldController.handleEvents(event, m_window);
        }
    }
}

void LandcraftEditor::handleContinuousEvents(const float deltaTime)
{
    m_uiController.handleContinuousEvents(deltaTime, m_window);
	m_worldController.handleContinuousEvents(deltaTime, m_window);
}