#include "ScreenMap.hpp"

#define PI 3.14159265358979323846
#define TILE_SIZE_X 64
#define TILE_SIZE_Y 64
#define HEIGHT_SCALE 6 // => 64 / 8
#define PROJECTION_ANGLE_X 45
#define PROJECTION_ANGLE_Y 35 // 35.264 realistic isometric angle

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Landcraft");
    ScreenMap screenMap(TILE_SIZE_X, TILE_SIZE_Y, HEIGHT_SCALE, sf::Vector2f{600, 100},
                        PROJECTION_ANGLE_X, PROJECTION_ANGLE_Y);
    screenMap.init("");
    SelectionMode selectionMode = SelectionMode::TILE_CORNER;
    int heightOffset = 1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                if (event.key.code == sf::Keyboard::Space)
                    selectionMode = (selectionMode == SelectionMode::TILE) 
                                    ? SelectionMode::TILE_CORNER 
                                    : SelectionMode::TILE;
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Z)
                    screenMap.setSelectedCornersHeight(heightOffset);
                if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                    screenMap.setSelectedCornersHeight(-heightOffset);
            }
        }
        window.clear();
        screenMap.update(window, selectionMode);
        screenMap.draw(window);
        window.display();
    }
    return 0;
}