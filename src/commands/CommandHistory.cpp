//
// Created by fran on 17/03/2026.
//

#include "CommandHistory.hpp"

CommandHistory::CommandHistory() 
    : m_currentIndex(-1)
{
}

void CommandHistory::addCommand(std::unique_ptr<ICommand> command, WorldModel& model, WorldView& view, bool shouldExecute)
{
    if (!m_commandList.empty() && m_currentIndex < static_cast<int>(m_commandList.size()) - 1) {
        auto eraseStart = m_commandList.begin() + (m_currentIndex + 1);
        m_commandList.erase(eraseStart, m_commandList.end());
    }
    if (shouldExecute)
        command->execute(model, view);
    addHistoryMessage(std::to_string(m_currentIndex + 1) + ": " + command->getName());
    m_commandList.push_back(std::move(command));
    m_currentIndex++;
    // current index store the index of the last executed command, 
    //so when we add a new command we need to move it forward and execute the command
}

void CommandHistory::undoCommand(WorldModel &model, WorldView &view)
{
    // we undo the current command (last executed command) and then move the index back,
    // so the current index will always point to the last executed command
    if (m_currentIndex == -1) {
        // message that there is no command to undo
        addHistoryMessage("No command to undo");
        return;
    }
    m_commandList[m_currentIndex--]->undo(model, view);
    addHistoryMessage(std::to_string(m_currentIndex + 1) + ": " + m_commandList[m_currentIndex + 1]->getName() + " undone");
    addHistoryMessage(std::to_string(m_currentIndex) + ": " + (m_currentIndex >= 0 ? m_commandList[m_currentIndex]->getName() : "No command") + " is now the current command");
}

void CommandHistory::redoCommand(WorldModel &model, WorldView &view)
{
    // we move the index forward and then redo the command,
    // so the current index will always point to the last executed command
    if ( m_currentIndex + 1 >= m_commandList.size()) {
        // message that there is no command to redo
        addHistoryMessage("No command to redo");
        return;
    }
    m_commandList[++m_currentIndex]->execute(model, view);
    addHistoryMessage(std::to_string(m_currentIndex) + ": " + m_commandList[m_currentIndex]->getName() + " redone");
}

void CommandHistory::clearHistory()
{
    m_commandList.clear();
    m_historyMessages.clear();
    m_currentIndex = -1;
}

void CommandHistory::addHistoryMessage(const std::string &message)
{
    m_historyMessages.push_back(message);
    std::cout << "Command history : " << message << std::endl;
}
