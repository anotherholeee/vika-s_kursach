#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <memory>
#include <vector>
#include "exceptions.h"

class TransportSystem;

// Базовый класс для команд (паттерн Command)
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getDescription() const = 0;
};

// Класс для управления историей команд
class CommandHistory {
private:
    std::vector<std::unique_ptr<Command>> history;
    size_t currentIndex = 0;
    static const size_t MAX_HISTORY_SIZE = 100;

public:
    void executeCommand(std::unique_ptr<Command> cmd);
    bool canUndo() const;
    void undo();
    bool canRedo() const;
    void redo();
    void clear();
    std::string getLastCommandDescription() const;
    std::string getNextCommandDescription() const;
};

#endif // COMMAND_H

