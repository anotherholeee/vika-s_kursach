#include "command.h"

void CommandHistory::executeCommand(std::unique_ptr<Command> cmd) {
    history.erase(history.begin() + currentIndex, history.end());
    cmd->execute();
    history.push_back(std::move(cmd));
    currentIndex = history.size();

    if (history.size() > MAX_HISTORY_SIZE) {
        history.erase(history.begin());
        currentIndex--;
    }
}

bool CommandHistory::canUndo() const {
    return currentIndex > 0;
}

void CommandHistory::undo() {
    if (!canUndo()) {
        throw ContainerException("Нет действий для отмены");
    }
    currentIndex--;
    history[currentIndex]->undo();
}

bool CommandHistory::canRedo() const {
    return currentIndex < history.size();
}

void CommandHistory::redo() {
    if (!canRedo()) {
        throw ContainerException("Нет действий для повтора");
    }
    history[currentIndex]->execute();
    currentIndex++;
}

void CommandHistory::clear() {
    history.clear();
    currentIndex = 0;
}

std::string CommandHistory::getLastCommandDescription() const {
    if (currentIndex > 0) {
        return history[currentIndex - 1]->getDescription();
    }
    return "Нет выполненных команд";
}

std::string CommandHistory::getNextCommandDescription() const {
    if (currentIndex < history.size()) {
        return history[currentIndex]->getDescription();
    }
    return "Нет действий для повтора";
}

