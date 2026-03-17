//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_WORLDCONTROLLER_HPP
#define LANDCRAFT_WORLDCONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/WorldView.hpp"
#include "EditionController.hpp"


struct CameraSettings {
    float tileSizeX;
    float tileSizeY;
    float heightScale;
    float projectionAngleX;
    float projectionAngleY;
};

struct ViewSettings {
    sf::Vector2f center;
    sf::Vector2f size;
    sf::Vector2u windowSize;
};

class WorldController 
{
public:
    WorldController();
    void init(const std::string &mapName, 
        const CameraSettings& cameraSettings, const ViewSettings& viewSettings);
    void handleEvents(sf::RenderWindow& window);
    void update(float deltaTime, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
private:
    void handlePanEvents(const sf::RenderWindow& window, const sf::Event &event);
    void handleContinuousPanEvents();
    void handleRotationEvents(sf::RenderWindow& window, const sf::Event &event);
    void handleZoomEvents(const sf::RenderWindow& window, const sf::Event &event);
    void handleMapEditingEvents(sf::RenderWindow& window, const sf::Event &event);
    void handleContinuousMapEditingEvents();
    void handleUndoRedoEvents(const sf::RenderWindow& window, const sf::Event &event);

    // add event for turn on/off wireframe, shaded mode,
    void updateSelectedCornersHeight(int heightStep);

    WorldModel m_worldModel;
    WorldView m_worldView;
    EditionController m_editionController;
    // SelectionController m_selectionController;
    // CommandHistory m_commandHistory;

    float m_movementStep;
    int m_zoomStep;
    float m_pitchRotationStep;
    float m_yawRotationStep;
    // int m_heightStep;
    bool m_isRotating;
    bool m_isMovementKeyPressed;
    // int m_currentTextureId;
    // SelectionMode m_currentSelectionMode;
};


#endif //LANDCRAFT_WORLDCONTROLLER_HPP