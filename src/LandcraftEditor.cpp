//
// Created by fran on 02/04/2026.
//

#include "LandcraftEditor.hpp"
#include "utils/FileUtils.hpp"

LandcraftEditor::LandcraftEditor(std::string mapName)
    : m_appState(mapName.empty() ? AppState::StartMenu : AppState::Editor)
    , m_isEditorInitialized(false)
    , m_startingMapName(mapName)
    , m_emptyMapName("assets/maps/landcraft-map.legend")
    , m_hasFocus(true)
    , m_windowSize(sf::Vector2f(1920, 990))
    , m_viewSize(m_windowSize)
    , m_tileSizeX(64)
    , m_tileSizeY(64)
    , m_heightScale(6) // => 64 / 8
    , m_projectionAngleX(30)
    , m_projectionAngleY(15) // 35.264 realistic isometric angle
    , m_window(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Landcraft", sf::Style::Default, sf::ContextSettings(0, 0, 4))
    , m_uiController(nullptr)
    , m_worldController(nullptr)
    , m_startMenu(nullptr)
    , m_helpMenu(nullptr)
{
    m_window.setPosition(sf::Vector2i(-9, 0));
    m_window.setVerticalSyncEnabled(true);
    applyWindowIcon();

    m_uiController = std::make_unique<UIController>();
    m_uiController->setOnDestroy([] {
        UIFactory::init(nullptr);
    });
    UIFactory::init(m_uiController.get());
    m_worldController = std::make_unique<WorldController>(sf::Vector2u(800, 600), 
        sf::Vector2u(static_cast<unsigned int>(m_windowSize.x), static_cast<unsigned int>(m_windowSize.y)));

    if (m_appState == AppState::StartMenu)
        initStartMenu();
    else if (m_appState == AppState::Editor)
        initWorldController();
}

void LandcraftEditor::run()
{
    float deltaTime = 0.0f;

    m_clock.restart();
    while (m_window.isOpen())
    {
        handleEvents();
        deltaTime = m_clock.restart().asSeconds();
        if (deltaTime > 0.1f) 
            deltaTime = 0.1f;

        if (m_hasFocus) {
            handleContinuousEvents(deltaTime);
            m_uiController->update(deltaTime, m_window);
            if (m_appState == AppState::Editor)
                m_worldController->update(deltaTime, m_window);
        }
        if (m_appState == AppState::Editor) {
            m_worldController->draw(m_window);
        } else 
            m_window.clear(sf::Color(235, 230, 250));
        m_uiController->draw(m_window);
        m_window.display();
    }
}

void LandcraftEditor::handleEvents()
{
    sf::Event event;

    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            m_window.close();
        else if (event.type == sf::Event::KeyPressed
            && event.key.code == sf::Keyboard::Escape
            && !m_uiController->isKeyBoardNavigatingHoverUI()) {
            if (m_appState == AppState::StartMenu)
                onCloseStartMenuRequested();
            else
                onCloseEditorRequested();
        }

        switch (event.type) {
            case sf::Event::LostFocus:
                m_hasFocus = false;
                break;
            case sf::Event::GainedFocus:
                m_hasFocus = true;
                m_clock.restart();
                applyWindowIcon();
                break;
            case sf::Event::Resized:
                m_windowSize = sf::Vector2u(event.size.width, event.size.height);
                if (m_appState == AppState::StartMenu) {
                    if (m_startMenu != nullptr)
                        m_startMenu->onWindowResized(sf::Vector2f(m_windowSize));
                    if (m_helpMenu != nullptr)
                        m_helpMenu->onWindowResized(sf::Vector2f(m_windowSize));
                } else {
                    m_worldController->onWindowResized(m_windowSize, m_window);
                }
                applyWindowIcon();
                break;
            default:
                break;
        }

        if (m_hasFocus) {
            m_uiController->handleEvents(event, m_window);
            if (m_appState == AppState::Editor
                && m_uiController->shouldForwardEventToWorld(event))
                m_worldController->handleEvents(event, m_window);
        }
    }
}

void LandcraftEditor::handleContinuousEvents(const float deltaTime) const
{
    m_uiController->handleContinuousEvents(deltaTime, m_window);
    if (m_appState == AppState::Editor && !m_uiController->isMouseHoverUI())
        m_worldController->handleContinuousEvents(deltaTime, m_window);
}

void LandcraftEditor::initStartMenu()
{
    const sf::Vector2f windowSize(static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y));
    m_startMenu = std::make_unique<StartMenu>(windowSize);
    m_helpMenu = std::make_unique<HelpMenu>(windowSize);

    m_startMenu->setNewProjectButtonOnClickCallback([this] () {
        m_startingMapName = m_emptyMapName;
        transitionToEditor();
    });
    m_startMenu->setLoadMapButtonOnClickCallback([this] () {
        onLoadMapRequested();
    });
    m_startMenu->setHelpButtonOnClickCallback([this] () {
        setHelpMenuVisibility(true);
    });
    m_helpMenu->setCloseButtonOnClickCallback([this] () {
        setHelpMenuVisibility(false);
    });
}

void LandcraftEditor::initWorldController()
{
    if (m_isEditorInitialized)
        return;

    const CameraSettings cameraSettings{
        m_tileSizeX, m_tileSizeY, m_heightScale, m_projectionAngleX, m_projectionAngleY
    };
    const ViewSettings viewSettings{
        sf::Vector2f{0, 0},
        sf::Vector2f{static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y)},
        m_windowSize
    };
    if (!m_appLoadingController.initializeWorld(m_window, m_windowSize, m_startingMapName,
        *m_worldController, cameraSettings, viewSettings)) {
        return;
    }

    m_worldController->setSaveMapButtonOnClickCallback([this] () {
        this->m_worldController->saveMapToFile();
        this->m_window.close();
    });
    m_worldController->setDontSaveButtonOnClickCallback([this] () {
        this->m_window.close();
    });
    m_worldController->setCancelButtonOnClickCallback([this] () {
        this->m_worldController->setQuitMenuVisibility(false);
    });

    m_isEditorInitialized = true;
}

void LandcraftEditor::transitionToEditor()
{
    setHelpMenuVisibility(false);
    if (m_startMenu != nullptr)
        m_startMenu->setVisibility(false);

    m_appState = AppState::Editor;
    initWorldController();
}

void LandcraftEditor::onLoadMapRequested()
{
    const std::vector<std::string> mapFileFilters = {
        "Legend Files", "*.legend"
    };
    const std::string openPath = FileUtils::getFileToOpenPathFromFileDialog(mapFileFilters);
    if (openPath.empty())
        return;
    m_startingMapName = openPath;
    transitionToEditor();
}

void LandcraftEditor::setHelpMenuVisibility(bool isVisible)
{
    if (m_helpMenu != nullptr)
        m_helpMenu->setVisibility(isVisible);
}

void LandcraftEditor::onCloseEditorRequested()
{
    if (m_worldController->isQuitMenuVisible()) {
        m_window.close();
        return;
    }
    m_worldController->setQuitMenuVisibility(true);
}

void LandcraftEditor::onCloseStartMenuRequested()
{
    if (m_helpMenu != nullptr && m_helpMenu->isVisible()) {
        setHelpMenuVisibility(false);
        return;
    }
    m_window.close();
}

void LandcraftEditor::applyWindowIcon()
{
    const sf::Image &icon = ResourceManager::getInstance().getImage("assets/textures/ui/landcraft_icon_512.png");
    const sf::Vector2u iconSize = icon.getSize();
    if (iconSize.x == 0 || iconSize.y == 0)
        return;
    m_window.setIcon(iconSize.x, iconSize.y, icon.getPixelsPtr());
}
