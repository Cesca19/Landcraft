//
// Created by fran on 24/03/2026.
//

#ifndef LANDCRAFT_IEDITIONTOOL_HPP
#define LANDCRAFT_IEDITIONTOOL_HPP

#include <SFML/Graphics.hpp>
#include "../model/WorldModel.hpp"
#include "../view/WorldView.hpp"
#include "../controller/SelectionController.hpp"
#include "../commands/CommandHistory.hpp"

class IEditionTool {
public:
    virtual ~IEditionTool() = default;
    virtual SelectionMode getRequiredSelectionMode() const = 0;
    virtual bool isSelectionLocked() const = 0;
    virtual void handleEvents(const sf::Event& event, WorldModel& model, WorldView& view,
                              SelectionController& selectionController, CommandHistory& history) = 0;
    virtual void handleContinuousEvents(WorldModel& model, WorldView& view,
                                        SelectionController& selectionController, CommandHistory& history) = 0;
};

#endif //LANDCRAFT_IEDITIONTOOL_HPP