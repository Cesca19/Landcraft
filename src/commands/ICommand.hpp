//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_ICOMMAND_HPP
#define LANDCRAFT_ICOMMAND_HPP

#include "../model/WorldModel.hpp"
#include "../view/world/WorldView.hpp"
#include "../controller/BrushController.hpp"

class ICommand {
public:
    virtual ~ICommand() {}
    virtual void execute(WorldModel& model, WorldView& view) = 0;
    virtual void undo(WorldModel& model, WorldView& view) = 0;
    virtual std::string getName() = 0;
};


#endif //LANDCRAFT_ICOMMAND_HPP