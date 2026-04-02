//
// Created by fran on 24/03/2026.
//

#ifndef LANDCRAFT_ELEVATIONTOOL_HPP
#define LANDCRAFT_ELEVATIONTOOL_HPP

#include "IEditionTool.hpp"
#include "../commands/EditTilesCornersHeightCommand.hpp"

class ElevationTool : public IEditionTool
{
public:
    ElevationTool();
    bool isEditing() const override;
    bool isSelectionLocked() const override;
    SelectionMode getRequiredSelectionMode() const override;
    void handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, SelectionController &selectionController, CommandHistory &history) override;
    void handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, SelectionController &selectionController, CommandHistory &history) override;
private:
    void handleSelectionModeEditingEvents(const sf::Event& event);
    void handleHeightEditingEvents(const sf::RenderWindow& window, WorldModel& model, WorldView& view, const SelectionController &selectionController, CommandHistory &history);
    void startContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const SelectionController &selectionController, float heightStep);
    void updateContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, 
        const SelectionController &selectionController, float heightStep);
    void stopContinuousElevation(WorldModel &model, WorldView &view, CommandHistory &history);
    std::set<TileCorner*> getTilesCornersFromBresenhamLine(sf::Vector2i startPosition, sf::Vector2i endPosition, WorldModel &model) const;

    float m_heightStep;

    bool m_isEditing;
    bool m_isSelectionLocked;
    SelectionMode m_currentSelectionMode;

    float m_continuousElevationInterval;
    sf::Clock m_continuousElevationClock;

    sf::Vector2i m_lastMouseScreenPosition;
    sf::Vector2i m_lastMouseWorldPosition;

    float m_mouseMovementThreshold;
    const sf::Mouse::Button m_editingMouseButton = sf::Mouse::Left;
    std::unique_ptr<EditTilesCornersHeightCommand> m_ongoingEditCornersHeightCommand;
};


#endif //LANDCRAFT_ELEVATIONTOOL_HPP