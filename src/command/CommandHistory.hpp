//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_COMMANDHISTORY_HPP
#define LANDCRAFT_COMMANDHISTORY_HPP

#include "ICommand.hpp"

class CommandHistory 
{
public:
    CommandHistory();
    void addCommand(std::unique_ptr<ICommand> command, WorldModel& model, WorldView& view);
    void undoCommand(WorldModel& model, WorldView& view);
    void redoCommand(WorldModel& model, WorldView& view);
private:
    void addHistoryMessage(const std::string& message);

    std::vector<std::unique_ptr<ICommand>> m_commandList;
    std::vector<std::string> m_historyMessages;
    int m_currentIndex;
};


#endif //LANDCRAFT_COMMANDHISTORY_HPP