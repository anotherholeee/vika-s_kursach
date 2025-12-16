#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <string>
#include <memory>
#include "journey.h"
#include "time.h"
#include "route.h"
#include "trip.h"

class TransportSystem;

// Базовый класс для алгоритмов (принцип полиморфизма)
class Algorithm {
protected:
    TransportSystem* system;

public:
    explicit Algorithm(TransportSystem* sys) : system(sys) {}
    virtual ~Algorithm() = default;

    // Виртуальный метод выполнения алгоритма
    virtual void execute() = 0;
    
    // Получение описания алгоритма
    virtual std::string getDescription() const = 0;
    
    // Установка системы (для работы с const методами)
    void setSystem(TransportSystem* sys) { system = sys; }
};

// Класс алгоритма поиска пути (Strategy pattern)
class PathFindingAlgorithm : public Algorithm {
public:
    explicit PathFindingAlgorithm(TransportSystem* sys) : Algorithm(sys) {}
    
    virtual std::vector<Journey> findPath(const std::string& start, 
                                         const std::string& end,
                                         const Time& departureTime) = 0;
    
    void execute() override {}
};

// Алгоритм поиска в ширину (BFS) для поиска маршрутов с пересадками
class BFSAlgorithm : public PathFindingAlgorithm {
private:
    int maxTransfers;

public:
    BFSAlgorithm(TransportSystem* sys, int maxTransfers = 2) 
        : PathFindingAlgorithm(sys), maxTransfers(maxTransfers) {}

    std::vector<Journey> findPath(const std::string& start, 
                                 const std::string& end,
                                 const Time& departureTime) override;

    void execute() override {
        // Реализация может быть добавлена при необходимости
    }

    std::string getDescription() const override {
        return "Алгоритм поиска в ширину (BFS) для поиска маршрутов с пересадками";
    }
};

// Алгоритм поиска самого быстрого маршрута
class FastestPathAlgorithm : public PathFindingAlgorithm {
public:
    explicit FastestPathAlgorithm(TransportSystem* sys) : PathFindingAlgorithm(sys) {}

    std::vector<Journey> findPath(const std::string& start, 
                                 const std::string& end,
                                 const Time& departureTime) override;

    void execute() override {}

    std::string getDescription() const override {
        return "Алгоритм поиска самого быстрого маршрута";
    }
};

// Алгоритм поиска маршрута с минимальными пересадками
class MinimalTransfersAlgorithm : public PathFindingAlgorithm {
public:
    explicit MinimalTransfersAlgorithm(TransportSystem* sys) : PathFindingAlgorithm(sys) {}

    std::vector<Journey> findPath(const std::string& start, 
                                 const std::string& end,
                                 const Time& departureTime) override;

    void execute() override {}

    std::string getDescription() const override {
        return "Алгоритм поиска маршрута с минимальными пересадками";
    }
};

// Алгоритм расчета времени прибытия
class ArrivalTimeCalculationAlgorithm : public Algorithm {
public:
    explicit ArrivalTimeCalculationAlgorithm(TransportSystem* sys) : Algorithm(sys) {}

    void calculateArrivalTimes(int tripId, double averageSpeed);

    void execute() override {
        // Реализация может быть добавлена при необходимости
    }

    std::string getDescription() const override {
        return "Алгоритм расчета времени прибытия на остановки";
    }
};

// Алгоритм поиска маршрутов между остановками
class RouteSearchAlgorithm : public Algorithm {
public:
    explicit RouteSearchAlgorithm(TransportSystem* sys) : Algorithm(sys) {}

    std::vector<std::shared_ptr<Route>> findRoutes(const std::string& stopA, 
                                                   const std::string& stopB);

    void execute() override {}

    std::string getDescription() const override {
        return "Алгоритм поиска маршрутов между остановками";
    }
};

#endif // ALGORITHM_H

