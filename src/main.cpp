//
// Created by fran on 03/03/2026.
//

#include "controller/WorldController.hpp"
#define PI 3.14159265358979323846
#define TILE_SIZE_X 64
#define TILE_SIZE_Y 64
#define HEIGHT_SCALE 6 // => 64 / 8
#define PROJECTION_ANGLE_X 30
#define PROJECTION_ANGLE_Y 15 // 35.264 realistic isometric angle


int main()
{
    // Get a different random number each time the program runs
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Landcraft");
    WorldController worldController;
    worldController.init("assets/maps/map.txt", 
        {TILE_SIZE_X, TILE_SIZE_Y, HEIGHT_SCALE, PROJECTION_ANGLE_X, PROJECTION_ANGLE_Y},
        {sf::Vector2f{0, 0}, sf::Vector2f{1200, 800}, window.getSize()}
    );
    sf::Clock clock;
    float deltaTime = 0;
    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        window.clear(sf::Color(196, 218, 242));

        worldController.handleEvents(window);
        worldController.update(deltaTime, window);
        worldController.draw(window);
        
        window.display();
    }
    return 0;
}