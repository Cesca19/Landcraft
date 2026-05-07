//
// Created by fran on 06/05/2026.
//

#ifndef LANDCRAFT_MAPLOADSAVECONTROLLER_HPP
#define LANDCRAFT_MAPLOADSAVECONTROLLER_HPP

#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"
#include "../utils/FileUtils.hpp"
#include "EditionController.hpp"
#include "../view/menu/MapSaveLoadMenu.hpp"

class MapLoadSaveController {
public:
    MapLoadSaveController(WorldModel *m_worldModel, WorldView *m_worldView, EditionController *m_editionController, sf::Vector2f globalUIPosition);
    void handleEvents(const sf::Event &event, sf::RenderWindow& window);
    void saveMapToFile();
    void loadMapFromFile();
private:
    void resetEditor();

    WorldView *m_worldView;
    WorldModel *m_worldModel;
    EditionController *m_editionController;
    std::unique_ptr<MapSaveLoadMenu> m_mapSaveLoadMenu;
    const std::vector<std::string> m_mapFileFilters = {
        "Legend Files", "*.legend"
    };
};


#endif //LANDCRAFT_MAPLOADSAVECONTROLLER_HPP