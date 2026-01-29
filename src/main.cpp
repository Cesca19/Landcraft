#include "WorldMap.hpp"
#include "ScreenMap.hpp"

#define PI 3.14159265358979323846
#define TILE_SIZE_X 32
#define TILE_SIZE_Y 32
#define HEIGHT_SCALE 6 // => 64 / 8
#define PROJECTION_ANGLE_X 45
#define PROJECTION_ANGLE_Y 35 // 35.264 realistic isometric angle

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Landcraft");
    WorldMap worldMap("");
    ScreenMap screenMap(TILE_SIZE_X, TILE_SIZE_Y, HEIGHT_SCALE, sf::Vector2f{400, 100}, 
                        PROJECTION_ANGLE_X, PROJECTION_ANGLE_Y);
    SelectionMode selectionMode = SelectionMode::TILE_CORNER;
    int heightOffset = 1;

    screenMap.buildScreenMap(worldMap.getMap());
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Space)
                    selectionMode = (selectionMode == SelectionMode::TILE) 
                                    ? SelectionMode::TILE_CORNER 
                                    : SelectionMode::TILE;
                // if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Z)
                //     worldMap.setSelectedTilesCornersHeight(heightOffset);
                // if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                //     worldMap.setSelectedTilesCornersHeight(-heightOffset);
            }
        }
        window.clear();
        screenMap.update(window);
        // worldMap.updateTilesState(screenMap.GetMouseWorldPosition(window), selectionMode);
        // worldMap.removeSelectedTilesCorners();
        // std::vector<sf::Vector2i> selectedTilesCorner = screenMap.getSelectedTilesCorner(window);
        // for (const sf::Vector2i& selectedTileCorner : selectedTilesCorner)
        //     worldMap.onTileCornerHovered(selectedTileCorner.x, selectedTileCorner.y);
        screenMap.draw(window);
        window.display();
    }

    return 0;
}