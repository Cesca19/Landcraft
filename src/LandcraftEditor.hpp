//
// Created by fran on 02/04/2026.
//

#ifndef LANDCRAFT_LANDCRAFTEDITOR_HPP
#define LANDCRAFT_LANDCRAFTEDITOR_HPP

#include "controller/WorldController.hpp"
#include "controller/EditorLoadingController.hpp"
#include "ui/UIController.hpp"
#include "ui/UIFactory.hpp"
#include "view/menu/StartMenu.hpp"
#include "view/menu/HelpMenu.hpp"

enum class AppState {
    StartMenu,
    Editor
};

class LandcraftEditor {
public:
   LandcraftEditor(std::string mapName = "");
    void run();
private:
    void applyWindowIcon();
    void handleEvents();
    void handleContinuousEvents(float deltaTime) const;
    void initStartMenu();
    void initWorldController();
    void transitionToEditor();
    void onLoadMapRequested();
    void setHelpMenuVisibility(bool isVisible);
    void onCloseEditorRequested();
    void onCloseStartMenuRequested();

    AppState m_appState;
    bool m_isEditorInitialized;
    std::string m_emptyMapName;
    std::string m_startingMapName;
    bool m_hasFocus;
    sf::Vector2u m_windowSize;
    sf::Vector2u m_viewSize;
    float m_tileSizeX;
    float m_tileSizeY;
    float m_heightScale;
    float m_projectionAngleX;
    float m_projectionAngleY;
    sf::Clock m_clock;
    sf::RenderWindow m_window;
    EditorLoadingController m_appLoadingController;
    std::unique_ptr<UIController> m_uiController;
    std::unique_ptr<WorldController> m_worldController;
    std::unique_ptr<StartMenu> m_startMenu;
    std::unique_ptr<HelpMenu> m_helpMenu;
};

#endif //LANDCRAFT_LANDCRAFTEDITOR_HPP
