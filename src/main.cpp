#include "WorldManager.hpp"
#define PI 3.14159265358979323846
#define TILE_SIZE_X 64
#define TILE_SIZE_Y 64
#define HEIGHT_SCALE 6 // => 64 / 8
#define PROJECTION_ANGLE_X 45
#define PROJECTION_ANGLE_Y 35 // 35.264 realistic isometric angle

int main()
{
    WorldManager world_manager(800, 600, "Landcraft", sf::Vector2f(1200, 800));
    world_manager.init("", TILE_SIZE_X, TILE_SIZE_Y, HEIGHT_SCALE, sf::Vector2f{0,0},
                        PROJECTION_ANGLE_X, PROJECTION_ANGLE_Y);
    world_manager.update();
    return 0;
}