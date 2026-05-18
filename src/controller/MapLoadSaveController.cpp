//
// Created by fran on 06/05/2026.
//

#include <exception>
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
        std::cout << "Save operation cancelled." << std::endl;
        return;
    }

    if (savePath.find(".legend") == std::string::npos)
        savePath += ".legend";
    try {
        std::string splatmapFilePath = getFilePathWithoutExtension(savePath) + "-splatmap.png";
        std::string splatmapFileName = getFileName(splatmapFilePath);
        TrySaveSplatmapImage(splatmapFilePath);
        m_worldModel->saveMapToFile(savePath, splatmapFileName);
        std::cout << "Map saved successfully." << std::endl;
    } catch (const std::exception& exception) {
        std::cerr << "Failed to save map: " << exception.what() << std::endl;
    }
}

void MapLoadSaveController::loadMapFromFile()
{
    std::string openPath = FileUtils::getFileToOpenPathFromFileDialog(m_mapFileFilters);

    if (openPath.empty()) {
        std::cout << "Load operation cancelled." << std::endl;
        return;
    }

    try {
        m_worldModel->loadMap(openPath);
        resetEditor();
        std::cout << "Map loaded successfully." << std::endl;
    } catch (const std::exception& exception) {
        std::cerr << "Failed to load map: " << exception.what() << std::endl;
    }
}

void MapLoadSaveController::resetEditor()
{
    m_editionController->clearCommandHistory();
    m_worldView->clearTileMap();
    m_worldView->resetTileMap(m_worldModel->getTiles());
    m_worldView->initSplatmap(m_worldModel->getSplatmapFilepath(), m_worldModel->getTilesSize(), m_worldModel->getMapSize().x, m_worldModel->getMapSize().y);
}

std::string MapLoadSaveController::getFilePathWithoutExtension(const std::string &filePath)
{
    size_t lastDot = filePath.find_last_of('.');
    if (lastDot != std::string::npos) {
        return filePath.substr(0, lastDot);
    }
    return filePath;
}

std::string MapLoadSaveController::getFileName(const std::string &filePath)
{
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(lastSlash + 1);
    }
    return filePath;
}

bool MapLoadSaveController::TrySaveSplatmapImage(const std::string &filepath)
{
    const sf::Image& splatmapImage = m_worldView->getSplatmapImage();
    if (splatmapImage.saveToFile(filepath)) {
        std::cout << "Splatmap image saved successfully to " << filepath << std::endl;
        return true;
    } else {
        std::cerr << "Failed to save splatmap image to " << filepath << std::endl;
        return false;
    }
}
