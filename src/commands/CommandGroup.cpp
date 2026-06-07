//
// Created by fran on 03/06/2026.
//

#include "CommandGroup.hpp"

CommandGroup::CommandGroup(const std::string &actionName)
    : m_actionName(actionName)
{
    m_actionName = "Group Command: " + m_actionName + " :";
    for (const auto &command : m_commands) {
        m_actionName += "\n\t->  " + command->getName();
    }
}

void CommandGroup::addCommand(std::unique_ptr<ICommand> command)
{
    m_commands.push_back(std::move(command));
}

void CommandGroup::execute(WorldModel &model, WorldView &view)
{
    for (const auto &command : m_commands)
        command->execute(model, view);
}

void CommandGroup::undo(WorldModel &model, WorldView &view)
{
    for (int i = m_commands.size() - 1; i >= 0; i--)
        m_commands[i]->undo(model, view);
}

std::string CommandGroup::getName()
{
    return m_actionName;
}