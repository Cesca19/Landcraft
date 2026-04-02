//
// Created by fran on 02/04/2026.
//

#include "LandcraftEditor.hpp"

LandcraftEditor::LandcraftEditor()
    : m_hasFocus(false)
    , m_windowSize(sf::Vector2f(1200, 800))
    , m_viewSize(m_windowSize)
    , m_tileSizeX(64)
    , m_tileSizeY(64)
    , m_heightScale(6) // => 64 / 8
    , m_projectionAngleX(30)
    , m_projectionAngleY(15) // 35.264 realistic isometric angle
    , m_window(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Landcraft")
{
    m_worldController.init("assets/maps/map.txt",
        {m_tileSizeX, m_tileSizeY, m_heightScale, m_projectionAngleX, m_projectionAngleY},
        {sf::Vector2f{0, 0}, sf::Vector2f{1200, 800}, m_windowSize});
}

void LandcraftEditor::run()
{
    m_clock.restart();
    float deltaTime = 0;
    while (m_window.isOpen())
    {
        deltaTime = m_clock.restart().asSeconds();
        handleEvents();

        m_window.clear(sf::Color(196, 218, 242));
        m_worldController.update(deltaTime, m_window);
        m_worldController.draw(m_window);

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
                break;
            default:
                break;
        }
        // if (m_hasFocus) {
            // ui ctrl events
            m_worldController.handleEvents(event, m_window);
        // }
    }
    // if (m_hasFocus) {
        m_worldController.handleContinuousEvents(m_window);
    // }
}
