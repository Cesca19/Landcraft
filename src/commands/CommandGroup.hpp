//
// Created by fran on 03/06/2026.
//

#ifndef LANDCRAFT_COMMANDGROUP_HPP
#define LANDCRAFT_COMMANDGROUP_HPP

#include <vector>
#include "ICommand.hpp"

class CommandGroup : public ICommand {
public:
    CommandGroup(const std::string& actionName);
    void addCommand(std::unique_ptr<ICommand> command);
    void execute(WorldModel &model, WorldView &view) override;
    void undo(WorldModel &model, WorldView &view) override;
    std::string getName() override;
private:
    std::vector<std::unique_ptr<ICommand>> m_commands;
    std::string m_actionName;
};

#endif //LANDCRAFT_COMMANDGROUP_HPP