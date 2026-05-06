//
// Created by fran on 06/05/2026.
//

#include "MapLoadSaveController.hpp"

MapLoadSaveController::MapLoadSaveController(WorldModel *m_worldModel, WorldView *m_worldView, EditionController *m_editionController, sf::Vector2f globalUIPosition)
    : m_worldModel(m_worldModel)
    , m_worldView(m_worldView)
    , m_editionController(m_editionController)
    , m_mapSaveLoadMenu(std::make_unique<MapSaveLoadMenu>(globalUIPosition))
{
    m_mapSaveLoadMenu->setSaveFileButtonOnClickCallback([this] () {
        this->saveMapToFile();
    });
    m_mapSaveLoadMenu->setLoadFileButtonOnClickCallback([this] () {
        this->loadMapFromFile();
    });
}

void MapLoadSaveController::handleEvents(const sf::Event &event, sf::RenderWindow &window)
{
    if (event.type == sf::Event::KeyPressed) {
        bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
        if (isCtrlPressed && event.key.code == sf::Keyboard::S)
            saveMapToFile();
        else if (isCtrlPressed && event.key.code == sf::Keyboard::O)
            loadMapFromFile();
    }
}

void MapLoadSaveController::saveMapToFile()
{
    std::string savePath = FileUtils::getFileToSavePathFromFileDialog(m_mapFileFilters);

    if (savePath.empty()) {
        std::cerr << "Save operation cancelled by user." << std::endl;
        return;
    }

    if (savePath.find(".legend") == std::string::npos)
        savePath += ".legend";
    std::cout << "Save path: " << savePath << std::endl;
    if (m_worldModel->saveMapToFile(savePath))
        std::cout << "Map saved successfully." << std::endl;
    else
        std::cerr << "Failed to save map." << std::endl;
}

void MapLoadSaveController::loadMapFromFile()
{
    std::string openPath = FileUtils::getFileToOpenPathFromFileDialog(m_mapFileFilters);

    if (openPath.empty()) {
        std::cerr << "Load operation cancelled by user." << std::endl;
        return;
    }

    if (m_worldModel->loadMap(openPath, false)) {
        resetEditor();
        std::cout << "Map loaded successfully." << std::endl;
    } else
        std::cerr << "Failed to load map." << std::endl;
}

void MapLoadSaveController::resetEditor()
{
    m_editionController->clearCommandHistory();
    m_worldView->clearTileMap();
    m_worldView->resetTileMap(m_worldModel->getTiles());
}
