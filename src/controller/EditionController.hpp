//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_EDITIONCONTROLLER_HPP
#define LANDCRAFT_EDITIONCONTROLLER_HPP

#include "SelectionController.hpp"
#include "../command/Commands.hpp"
#include "../command/CommandHistory.hpp"
#include "../model/WorldModel.hpp"
#include "../view/WorldView.hpp"

class EditionController {
public:
    EditionController();
    void handleEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void handleContinuousEvents(sf::RenderWindow& window, WorldModel& model, WorldView& view);
    void update(float deltaTime, sf::RenderWindow& window, WorldModel& model, WorldView& view, bool isNavigating);
    void draw(sf::RenderWindow& window, const Camera& camera, bool isNavigating);
private:
    void handleUndoRedoEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void handleSelectionEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void handleTilePaintingEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void handleHeightEditingEvents(sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view);
    void updateSelectedCornersHeight(WorldModel& model, WorldView& view, int heightStep);
    std::vector<sf::Vector2i> getBresenhamLine(sf::Vector2i start, sf::Vector2i end) const;

    SelectionController m_selectionController;
    CommandHistory m_commandHistory;

    int m_heightStep;
    int m_currentTextureId;
    SelectionMode m_currentSelectionMode;
    const sf::Mouse::Button m_paintMouseButton = sf::Mouse::Left;
    std::unique_ptr<PaintTilesCommand> m_ongoingPaintCommand;
    Tile* m_lastPaintedTile;
};

#endif //LANDCRAFT_EDITIONCONTROLLER_HPP