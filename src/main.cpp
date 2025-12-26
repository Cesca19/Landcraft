#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

#define PI 3.14159265358979323846
#define TILE_SIZE_X 64
#define TILE_SIZE_Y 64
#define HEIGHT_SCALE 6 // => 64 / 8

float radToDeg(float rad)
{
    return rad * 180 / PI;
}

float degToRad(float deg)
{
    return deg * PI / 180;
}

sf::Vector2f project_iso_point(int x , int y, int z)
{
    float angleX = degToRad(45);
    float angleY = degToRad(35);
    sf::Vector2f point2d;

    point2d.x = std::cos(angleX) * x - std::cos(angleX) * y ;
    point2d.y = std::sin(angleY) * y + std::sin(angleY) * x - z * HEIGHT_SCALE;
    return point2d;
}

std::vector<std::vector<sf::Vector2f>> createMap2d(std::vector<std::vector<int>> map3d, sf::Vector2f translationOffset)
{
    std::vector<std::vector<sf::Vector2f>> map2d;

    for (int y = 0; y < map3d.size(); y++) {
        std::vector<sf::Vector2f> row;
        for (int x = 0; x < map3d[y].size(); x++) {
            int worldX = x * TILE_SIZE_X;
            int worldY = y * TILE_SIZE_Y;
            row.push_back(project_iso_point(worldX, worldY, map3d[y][x]) + translationOffset);
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

sf::VertexArray buildMap2d(std::vector<std::vector<sf::Vector2f>> map2d)
{
    sf::VertexArray map(sf::Lines);
    sf::Color vertexColor = sf::Color::White;
    for (int y = 0; y < map2d.size(); y++) {
        for (int x = 0; x < map2d[y].size(); x++) {
            std::vector<sf::Vector2f> neighbors = getPointNeighbors(map2d, x, y);
            for (sf::Vector2f neighbor : neighbors) {
                map.append(sf::Vertex(map2d[y][x], vertexColor));
                map.append(sf::Vertex(neighbor, sf::Color::Green));
            }
        }
    }
    return map;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    std::vector<std::vector<int>> map3d = {
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 5, 3, 0},
        {0, 9, 0, 0, 0, 0},
        {0, 7, 5, 0, 0, 0},
        {0, 0, 0, 0, 0, 0}
    };
    std::vector<std::vector<sf::Vector2f>> map2d = createMap2d(map3d, sf::Vector2f{400, 100});
    sf::VertexArray gameMap = buildMap2d(map2d);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(gameMap);
        window.display();
    }

    return 0;
}