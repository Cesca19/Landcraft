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
    bool isSelectionLocked() const override;
    SelectionMode getRequiredSelectionMode() const override;
    void handleEvents(const sf::Event &event, WorldModel &model, WorldView &view, SelectionController &selectionController, CommandHistory &history) override;
    void handleContinuousEvents(WorldModel &model, WorldView &view, SelectionController &selectionController, CommandHistory &history) override;
private:
    void handleSelectionEvents(const sf::Event& event);
    void handleHeightEditingEvents(const sf::Event& event, WorldModel& model, WorldView& view,
                                 const SelectionController &selectionController, CommandHistory &history);
    static void updateSelectedCornersHeight(WorldModel& model, WorldView& view, int heightStep,
                                            const SelectionController &selectionController, CommandHistory &history);
    int m_heightStep;
    bool m_isSelectionLocked;
    SelectionMode m_currentSelectionMode;
    std::unique_ptr<EditTilesCornersHeightCommand> m_ongoingEditCornersHeightCommand;
};


#endif //LANDCRAFT_ELEVATIONTOOL_HPP