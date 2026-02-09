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
    WorldManager(int width, int height, const std::string &windowTitle);
    ~WorldManager();
    void init(const std::string worldMapFilePath, int tileSizeX, int tileSizeY, int heightScale,
        int projectionAngleX, int projectionAngleY);
    void update();
private:
    void handleEvents();
    sf::RenderWindow m_window;

    std::unique_ptr<WorldView> m_worldView;
    std::unique_ptr<ScreenMap> m_screenMap;
    SelectionMode m_currentSelectionMode;
    // used te define the amount of height to add in WorldSpace coordinates (tiles grid)
    int m_heightOffset;

    // used to define the dir of the zoom
    int m_zoomStep;

    // rotation angle to add to the current rotation
    float m_yawRotationStep;
    float m_pitchRotationStep;
    // used to define the dir off the movement
    float m_movementStep;
};


#endif //LANDCRAFT_WORLDMANAGER_H