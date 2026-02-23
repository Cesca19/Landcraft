#include "WorldManager.hpp"
#define PI 3.14159265358979323846
#define TILE_SIZE_X 64
#define TILE_SIZE_Y 64
#define HEIGHT_SCALE 6 // => 64 / 8
#define PROJECTION_ANGLE_X 30
#define PROJECTION_ANGLE_Y 15 // 35.264 realistic isometric angle

int main()
{
    WorldManager world_manager(1200, 800, "Landcraft");
    world_manager.init("", TILE_SIZE_X, TILE_SIZE_Y, HEIGHT_SCALE,
                        PROJECTION_ANGLE_X, PROJECTION_ANGLE_Y);
    world_manager.update();
    return 0;
}