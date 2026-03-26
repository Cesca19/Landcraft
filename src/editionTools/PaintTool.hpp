//
// Created by fran on 24/03/2026.
//

#ifndef LANDCRAFT_PAINTTOOL_HPP
#define LANDCRAFT_PAINTTOOL_HPP

#include "IEditionTool.hpp"
#include "../commands/PaintTilesCommand.hpp"

class PaintTool : public IEditionTool
{
public:
    PaintTool();
    bool isSelectionLocked() const override;
    SelectionMode getRequiredSelectionMode() const override;
    void handleEvents(const sf::RenderWindow& window, const sf::Event& event, WorldModel& model, WorldView& view,
                              SelectionController& selectionController, CommandHistory& history) override;
    void handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, SelectionController &selectionController, CommandHistory &history) override;
private:
    int m_currentTextureId;
    Tile* m_lastPaintedTile;
    std::unique_ptr<PaintTilesCommand> m_ongoingPaintCommand;
    const sf::Mouse::Button m_paintMouseButton = sf::Mouse::Left;
};


#endif //LANDCRAFT_PAINTTOOL_HPP