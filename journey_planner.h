#ifndef JOURNEY_PLANNER_H
#define JOURNEY_PLANNER_H

#include <vector>
#include <string>
#include <memory>
#include "journey.h"
#include "time.h"
#include "algorithm.h"

class TransportSystem;

// Класс планировщика поездок, использующий алгоритмы (Facade pattern)
class JourneyPlanner {
private:
    TransportSystem* system;
    
    // Алгоритмы (Strategy pattern)
    std::unique_ptr<BFSAlgorithm> bfsAlgorithm;
    std::unique_ptr<FastestPathAlgorithm> fastestAlgorithm;
    std::unique_ptr<MinimalTransfersAlgorithm> minimalTransfersAlgorithm;

public:
    JourneyPlanner(TransportSystem* sys);

    std::vector<Journey> findJourneysWithTransfers(const std::string& startStop,
                                                   const std::string& endStop,
                                                   const Time& departureTime,
                                                   int maxTransfers = 2) const;

    std::vector<Journey> findAllJourneysWithTransfers(const std::string& startStop,
                                                      const std::string& endStop,
                                                      int maxTransfers = 2) const;

    Journey findFastestJourney(const std::string& startStop,
                               const std::string& endStop,
                               const Time& departureTime);

    Journey findJourneyWithLeastTransfers(const std::string& startStop,
                                          const std::string& endStop,
                                          const Time& departureTime);

    void displayJourney(const Journey& journey) const;
};

#endif // JOURNEY_PLANNER_H

