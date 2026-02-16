//
// Created by fran on 03/02/2026.
//

#include "WorldManager.hpp"

WorldManager::WorldManager(const int width, const int height, const std::string &windowTitle)
    : m_window(sf::VideoMode(width, height), windowTitle)
    , m_worldView(std::make_unique<WorldView>(sf::Vector2f({0, 0}),
                                              sf::Vector2f({static_cast<float>(width), static_cast<float>(height)})))
    , m_screenMap(nullptr)
    , m_currentSelectionMode(SelectionMode::TILE_CORNER)
    , m_heightOffset(1)
    , m_zoomStep(1)
    , m_yawRotationStep(90)
    , m_pitchRotationStep(5)
    , m_movementStep(5)
    , m_projectionAngleX(0)
    , m_projectionAngleY(0)
{
}

WorldManager::~WorldManager()
{
}

void WorldManager::init(const std::string worldMapFilePath, int tileSizeX, int tileSizeY, int heightScale, int projectionAngleX, int projectionAngleY)
{
    m_projectionAngleX = projectionAngleX;
    m_projectionAngleY = projectionAngleY;
    
    m_screenMap = std::make_unique<ScreenMap>(tileSizeX, tileSizeY, heightScale, projectionAngleX, projectionAngleY);
    m_screenMap->init(worldMapFilePath);
    m_worldView->init(m_window);
    m_worldView->setOrigin(m_screenMap->getScreenMapCenter());
    // m_movementStep *= std::max(tileSizeX, tileSizeY);
}

void WorldManager::update()
{
    sf::Clock clock;
    float deltaTime = 0;
    while (m_window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        handleEvents();
        m_window.clear();
        m_worldView->update(deltaTime);
        m_screenMap->update(deltaTime, m_window, m_currentSelectionMode);
        // m_worldView->setOrigin(m_screenMap->getScreenMapCenter());
        m_screenMap->draw(m_window);
        m_window.display();
    }
}

void WorldManager::handleEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            m_window.close();
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::Escape)
                m_window.close();

            // map editing event
            if (event.key.code == sf::Keyboard::Space)
                m_currentSelectionMode = (m_currentSelectionMode == SelectionMode::TILE)
                                ? SelectionMode::TILE_CORNER
                                : SelectionMode::TILE;
            if (event.key.code == sf::Keyboard::Add)
                m_screenMap->setSelectedCornersHeight(m_heightOffset);
            if (event.key.code == sf::Keyboard::Subtract)
                m_screenMap->setSelectedCornersHeight(- m_heightOffset);

            // cam events
            // zoom
            if (event.key.code == sf::Keyboard::I)
                m_worldView->zoom(-m_zoomStep);
            if (event.key.code == sf::Keyboard::O)
                m_worldView->zoom(m_zoomStep);
            // yaw
            if ( event.key.code == sf::Keyboard::Right)
                m_screenMap->rotateAroundZAxis(m_yawRotationStep);
            if (event.key.code == sf::Keyboard::Left)
                m_screenMap->rotateAroundZAxis(-m_yawRotationStep);
            // pitch
            if (event.key.code == sf::Keyboard::Up)
                m_screenMap->rotateAroundXAxis(m_pitchRotationStep);
            if (event.key.code == sf::Keyboard::Down)
                m_screenMap->rotateAroundXAxis(-m_pitchRotationStep);
        }

        // move events - Navigation 3D isométrique
        float forward = 0.f;  // Mouvement en profondeur (axe Y 3D)
        float right = 0.f;    // Mouvement latéral (axe X 3D)
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            forward += 1.f;   // Avancer dans le décor
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            forward -= 1.f;   // Reculer
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            right += 1.f;     // Se déplacer à droite
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            right -= 1.f;     // Se déplacer à gauche
            
        if (forward != 0.f || right != 0.f) {
            sf::Vector2f moveVector = get3DMovementVector(forward, right);
            m_worldView->translate(moveVector * m_movementStep);
        }

        // camera events
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {m_worldView->zoom(event.mouseWheelScroll.delta);}
        }
    }
}

sf::Vector2f WorldManager::get3DMovementVector(float forward, float right) const
{
    // Convertir les angles en radians
    const float angleXRad = m_projectionAngleX * M_PI / 180.0f;
    const float angleYRad = m_projectionAngleY * M_PI / 180.0f;
    
    // Récupérer la rotation actuelle de la carte (yaw)
    const float currentYaw = m_screenMap->getCurrentYawRotation() * M_PI / 180.0f;
    
    // Calculer les composantes trigonométriques
    const float cosAngleX = std::cos(angleXRad);
    const float sinAngleY = std::sin(angleYRad);
    
    // Appliquer la rotation Yaw aux directions de mouvement
    // Cela fait que les touches de mouvement "suivent" la rotation de la carte
    const float rotatedForward = forward * std::cos(currentYaw) - right * std::sin(currentYaw);
    const float rotatedRight = forward * std::sin(currentYaw) + right * std::cos(currentYaw);
    
    // Convertir le mouvement 3D en vecteur 2D écran
    sf::Vector2f moveVector(0.f, 0.f);
    
    // Mouvement en profondeur (forward/backward sur axe Y 3D)
    // En iso : Y+ 3D → vers haut-droite écran
    moveVector.x += rotatedForward * cosAngleX;
    moveVector.y -= rotatedForward * sinAngleY;  // Négatif car Y écran va vers le bas
    
    // Mouvement latéral (left/right sur axe X 3D)  
    // En iso : X+ 3D → vers haut-gauche écran
    moveVector.x -= rotatedRight * cosAngleX;
    moveVector.y -= rotatedRight * sinAngleY;
    
    return moveVector;
}