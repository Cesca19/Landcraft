#include "../WorldManager.hpp"
#define PI 3.14159265358979323846
#define TILE_SIZE_X 64
#define TILE_SIZE_Y 64
#define HEIGHT_SCALE 6 // => 64 / 8
#define PROJECTION_ANGLE_X 30
#define PROJECTION_ANGLE_Y 15 // 35.264 realistic isometric angle



int main()
{
    sf::RenderWindow window(sf::VideoMode(512, 256), "Tilemap");
    const int level[] =
    {
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
        0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
        0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
        0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
        2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
        0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
    };

    TileMap map;
    if (!map.load("assets/tilemap-tileset.png", sf::Vector2u(32, 32), level, 16, 8))
        return -1;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(map);
        window.display();
    }
    return 0;
}

// int main()
// {
//     WorldManager world_manager(1200, 800, "Landcraft");
//     world_manager.init("", TILE_SIZE_X, TILE_SIZE_Y, HEIGHT_SCALE,
//                         PROJECTION_ANGLE_X, PROJECTION_ANGLE_Y);
//     world_manager.update();
//     return 0;
// }