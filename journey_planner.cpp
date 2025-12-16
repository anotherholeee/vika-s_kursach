#include "journey_planner.h"
#include "transport_system.h"
#include <queue>
#include <algorithm>
#include "exceptions.h"

JourneyPlanner::JourneyPlanner(TransportSystem* sys) 
    : system(sys),
      bfsAlgorithm(std::make_unique<BFSAlgorithm>(sys, 2)),
      fastestAlgorithm(std::make_unique<FastestPathAlgorithm>(sys)),
      minimalTransfersAlgorithm(std::make_unique<MinimalTransfersAlgorithm>(sys)) {}

std::vector<Journey> JourneyPlanner::findJourneysWithTransfers(
    const std::string& startStop,
    const std::string& endStop,
    const Time& departureTime,
    int maxTransfers) const {
    
    // Используем алгоритм BFS (создаем временный объект для const метода)
    BFSAlgorithm bfs(const_cast<TransportSystem*>(system), maxTransfers);
    return bfs.findPath(startStop, endStop, departureTime);
}

std::vector<Journey> JourneyPlanner::findAllJourneysWithTransfers(
    const std::string& startStop,
    const std::string& endStop,
    int maxTransfers) const {

    std::vector<Journey> journeys;

    struct SearchNode {
        std::string currentStop;
        Time currentTime;
        Time startTime;
        std::vector<std::shared_ptr<Trip>> pathTrips;
        std::vector<std::string> transferPoints;
        int transfers;
    };

    auto initialTrips = system->getTripsThroughStop(startStop);

    std::sort(initialTrips.begin(), initialTrips.end(),
              [&startStop](const auto& a, const auto& b) {
                  return a->getArrivalTime(startStop) < b->getArrivalTime(startStop);
              });

    std::queue<SearchNode> q;

    for (const auto& trip : initialTrips) {
        Time arrivalAtStart = trip->getArrivalTime(startStop);
        q.push({startStop, arrivalAtStart, arrivalAtStart, {}, {}, 0});
    }

    while (!q.empty()) {
        auto node = q.front();
        q.pop();

        if (node.currentStop == endStop) {
            journeys.emplace_back(node.pathTrips, node.transferPoints,
                                 node.startTime, node.currentTime);
            continue;
        }

        if (node.transfers >= maxTransfers) {
            continue;
        }

        auto trips = system->getTripsThroughStop(node.currentStop);

        for (const auto& trip : trips) {
            Time arrivalAtStop = trip->getArrivalTime(node.currentStop);

            if (node.currentStop != startStop && arrivalAtStop < node.currentTime) {
                continue;
            }

            if (!node.pathTrips.empty() && node.pathTrips.back() == trip) {
                continue;
            }

            const auto& routeStops = trip->getRoute()->getAllStops();
            int currentPos = trip->getRoute()->getStopPosition(node.currentStop);

            if (currentPos == -1) continue;

            for (int i = currentPos + 1; i < routeStops.size(); ++i) {
                std::string nextStop = routeStops[i];
                Time arrivalAtNext = trip->getArrivalTime(nextStop);

                SearchNode nextNode = node;
                nextNode.currentStop = nextStop;
                nextNode.currentTime = arrivalAtNext;
                nextNode.pathTrips.push_back(trip);

                if (nextNode.pathTrips.size() == 1) {
                    nextNode.startTime = arrivalAtStop;
                }

                if (!node.pathTrips.empty()) {
                    nextNode.transferPoints.push_back(node.currentStop);
                    nextNode.transfers++;
                }

                q.push(nextNode);
            }
        }
    }

    std::sort(journeys.begin(), journeys.end(),
              [](const Journey& a, const Journey& b) {
                  if (a.getStartTime() != b.getStartTime()) {
                      return a.getStartTime() < b.getStartTime();
                  }
                  return a.getTotalDuration() < b.getTotalDuration();
              });

    return journeys;
}

Journey JourneyPlanner::findFastestJourney(const std::string& startStop,
                                          const std::string& endStop,
                                          const Time& departureTime) {
    auto journeys = fastestAlgorithm->findPath(startStop, endStop, departureTime);

    if (journeys.empty()) {
        throw ContainerException("Маршрут не найден");
    }

    return journeys[0];
}

Journey JourneyPlanner::findJourneyWithLeastTransfers(const std::string& startStop,
                                                     const std::string& endStop,
                                                     const Time& departureTime) {
    auto journeys = minimalTransfersAlgorithm->findPath(startStop, endStop, departureTime);

    if (journeys.empty()) {
        throw ContainerException("Маршрут не найден");
    }

    return journeys[0];
}

void JourneyPlanner::displayJourney(const Journey& journey) const {
    journey.display();
}

