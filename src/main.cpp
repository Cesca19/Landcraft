#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>

#define PI 3.14159265358979323846
#define TILE_SIZE_X 32
#define TILE_SIZE_Y 32
#define HEIGHT_SCALE 6 // => 64 / 8
#define PROJECTION_ANGLE_X 45
#define PROJECTION_ANGLE_Y 35 // 35.264 realistic isometric angle

struct Corner {
    sf::Vector2f ScreenPosition;
    sf::Vector2i WorldPosition;
    int Height;
    sf::Color Color;
};

float radToDeg(float rad)
{
    return rad * 180 / PI;
}

float degToRad(float deg)
{
    return deg * PI / 180;
}

sf::Vector2f world_to_screen(int point3dX , int point3dY, int point3dZ)
{
    float angleX = degToRad(PROJECTION_ANGLE_X);
    float angleY = degToRad(PROJECTION_ANGLE_Y);
    sf::Vector2f point2d;

    point2d.x = std::cos(angleX) * point3dX - std::cos(angleX) * point3dY ;
    point2d.y = std::sin(angleY) * point3dY + std::sin(angleY) * point3dX - point3dZ * HEIGHT_SCALE;
    return point2d;
}

sf::Vector2f screen_to_world(int point2dX , int point2dY, int point2dZ)
{
    float angleX = degToRad(PROJECTION_ANGLE_X);
    float angleY = degToRad(PROJECTION_ANGLE_Y);
    sf::Vector2f point3d;

    point3d.x = 0.5f * ((point2dX / std::cos(angleX)) + (point2dY + point2dZ * HEIGHT_SCALE) / std::sin(angleY));
    point3d.y = 0.5f * (-(point2dX / std::cos(angleX)) + (point2dY + point2dZ * HEIGHT_SCALE) / std::sin(angleY));
    return point3d;
}

std::vector<std::vector<sf::Vector2f>> convertTo2dScreenMap(std::vector<std::vector<int>> input3dMap, sf::Vector2f translationOffset)
{
    std::vector<std::vector<sf::Vector2f>> map2d;

    for (int y = 0; y < input3dMap.size(); y++) {
        std::vector<sf::Vector2f> row;
        for (int x = 0; x < input3dMap[y].size(); x++) {
            int worldX = x * TILE_SIZE_X;
            int worldY = y * TILE_SIZE_Y;
            row.push_back(world_to_screen(worldX, worldY, input3dMap[y][x]) + translationOffset);
        }
        map2d.push_back(row);
    }
    return map2d;
}

std::vector<sf::Vector2f> getPointNeighbors(std::vector<std::vector<sf::Vector2f>> map2d, int x, int y)
{
    std::vector<sf::Vector2f> neighbors;
    if (x + 1 < map2d[y].size())
        neighbors.push_back(map2d[y][x + 1]);
    if (y + 1 < map2d.size())
        neighbors.push_back(map2d[y + 1][x]);
    return neighbors;
}

sf::VertexArray createVertexArrayMap(std::vector<std::vector<sf::Vector2f>> map2d)
{
    sf::VertexArray map(sf::Lines);
    sf::Color vertexColor = sf::Color::Cyan;
    for (int y = 0; y < map2d.size(); y++) {
        for (int x = 0; x < map2d[y].size(); x++) {
            std::vector<sf::Vector2f> neighbors = getPointNeighbors(map2d, x, y);
            for (sf::Vector2f neighbor : neighbors) {
                map.append(sf::Vertex(map2d[y][x], vertexColor));
                map.append(sf::Vertex(neighbor, vertexColor));
            }
        }
    }
    return map;
}

std::vector<std::vector<Corner>> buildCornersMap(std::vector<std::vector<int>> input3dMap, sf::Vector2f translationOffset)
{
    std::vector<std::vector<Corner>> cornersMap;

    for (int y = 0; y < input3dMap.size(); y++) {
        std::vector<Corner> row;
        for (int x = 0; x < input3dMap[y].size(); x++) {
            int worldX = x * TILE_SIZE_X;
            int worldY = y * TILE_SIZE_Y;
            sf::Vector2f screenPos = world_to_screen(worldX, worldY, input3dMap[y][x]) + translationOffset;
            Corner corner = {screenPos, sf::Vector2i(worldX, worldY), input3dMap[y][x], sf::Color::Cyan};
            row.push_back(corner);
        }
        cornersMap.push_back(row);
    }
    return cornersMap;
}

std::vector<sf::Vector2f> getPointNeighbors(std::vector<std::vector<Corner>> corners, int x, int y)
{
    std::vector<sf::Vector2f> neighbors;
    if (x + 1 < corners[y].size())
        neighbors.push_back(corners[y][x + 1].ScreenPosition);
    if (y + 1 < corners.size())
        neighbors.push_back(corners[y + 1][x].ScreenPosition);
    return neighbors;
}

sf::VertexArray createVertexArrayMap(std::vector<std::vector<Corner>> worldMap)
{
    sf::VertexArray map(sf::Lines);
    for (int y = 0; y < worldMap.size(); y++) {
        for (int x = 0; x < worldMap[y].size(); x++) {
            std::vector<sf::Vector2f> neighbors = getPointNeighbors(worldMap, x, y);
            for (sf::Vector2f neighbor : neighbors) {
                map.append(sf::Vertex(worldMap[y][x].ScreenPosition, worldMap[y][x].Color));
                map.append(sf::Vertex(neighbor, worldMap[y][x].Color));
            }
        }
    }
    return map;
}

sf::Vector2i GetMouseWorldPosition(sf::RenderWindow& window, sf::Vector2f translationOffset)
{
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

    sf::Vector2f worldPosition = screen_to_world(mousePosition.x - translationOffset.x, mousePosition.y - translationOffset.y, 0);
    int tileX = (worldPosition.x) / TILE_SIZE_X;
    int tileY = (worldPosition.y) / TILE_SIZE_Y;
    // std::cout << "Mouse World Position: (" << worldPosition.x << ", " << worldPosition.y << ")\n";
    std::cout << "Mouse Tile Position: (" << tileX << ", " << tileY << ")\n";
    return sf::Vector2i(tileX, tileY);
}

std::vector<std::vector<Corner>> updateCornerMap(std::vector<std::vector<Corner>> &cornersMap, sf::Vector2i mouseWorldPos)
{
    if (mouseWorldPos.x < 0 || mouseWorldPos.y < 0 || 
        mouseWorldPos.y >= cornersMap.size() || 
        mouseWorldPos.x >= cornersMap[0].size())
        return cornersMap;

    // Reset all colors to default
    for (int y = 0; y < cornersMap.size(); y++) {
        for (int x = 0; x < cornersMap[y].size(); x++) {
            cornersMap[y][x].Color = sf::Color::Cyan;
        }
    }
    cornersMap[mouseWorldPos.y][mouseWorldPos.x].Color = sf::Color::Yellow;
    return cornersMap;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Landcraft");
    std::vector<std::vector<int>> input3dMap = {
        // {0,0,0,0,0,0},
        // {0,0,0,0,0,0},
        // {0,0,0,5,3,0},
        // {0,0,0,0,0,0},
        // {0,0,0,0,0,0},
        // {0,0,0,0,0,0}
        
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 0, 5, 3, 0, 0, 1, 1, 0},
        {0, 9, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 7, 5, 0, 0, 0, 0, 1, 0, 0},
        {0, 3, 6, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    //std::vector<std::vector<sf::Vector2f>> map2d = convertTo2dScreenMap(input3dMap, sf::Vector2f{400, 100});
    //sf::VertexArray gameMap = createVertexArrayMap(map2d);
    std::vector<std::vector<Corner>> cornersMap = buildCornersMap(input3dMap, sf::Vector2f{400, 100});
    sf::VertexArray gameMap = createVertexArrayMap(cornersMap);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        sf::Vector2i mouseWorldPos = GetMouseWorldPosition(window, sf::Vector2f{400, 100});
        cornersMap = updateCornerMap(cornersMap, mouseWorldPos);
        gameMap = createVertexArrayMap(cornersMap);
        window.draw(gameMap);
        window.display();
    }

    return 0;
}