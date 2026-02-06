//
// Created by fran on 03/02/2026.
//

#ifndef LANDCRAFT_WORLDMANAGER_H
#define LANDCRAFT_WORLDMANAGER_H

#include "ScreenMap.hpp"
#include "WorldView.hpp"

class WorldManager
{
public:
    WorldManager(int width, int height, std::string windowTitle);
    ~WorldManager();
    void init(const std::string worldMapFilePath, int tileSizeX, int tileSizeY, int heightScale,
        sf::Vector2f translationOffset, int projectionAngleX, int projectionAngleY);
    void update();
private:
    void handleEvents();
    sf::RenderWindow m_window;

    std::unique_ptr<WorldView> m_worldView;
    std::unique_ptr<ScreenMap> m_screenMap;
    SelectionMode m_currentSelectionMode;
    int m_heightOffset;
};


#endif //LANDCRAFT_WORLDMANAGER_H