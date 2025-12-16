#include "algorithm.h"
#include "transport_system.h"
#include "exceptions.h"
#include <queue>
#include <algorithm>

std::vector<Journey> BFSAlgorithm::findPath(const std::string& start, 
                                           const std::string& end,
                                           const Time& departureTime) {
    std::vector<Journey> journeys;

    struct SearchNode {
        std::string currentStop;
        Time currentTime;
        std::vector<std::shared_ptr<Trip>> pathTrips;
        std::vector<std::string> transferPoints;
        int transfers;
    };

    std::queue<SearchNode> q;
    q.push({start, departureTime, {}, {}, 0});

    while (!q.empty()) {
        auto node = q.front();
        q.pop();

        if (node.currentStop == end) {
            journeys.emplace_back(node.pathTrips, node.transferPoints,
                                 departureTime, node.currentTime);
            continue;
        }

        if (node.transfers >= maxTransfers) {
            continue;
        }

        auto trips = system->getTripsThroughStop(node.currentStop);

        for (const auto& trip : trips) {
            Time arrivalAtStop = trip->getArrivalTime(node.currentStop);

            if (arrivalAtStop < node.currentTime) {
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
                  return a.getTotalDuration() < b.getTotalDuration();
              });

    return journeys;
}

std::vector<Journey> FastestPathAlgorithm::findPath(const std::string& start, 
                                                   const std::string& end,
                                                   const Time& departureTime) {
    BFSAlgorithm bfs(system, 2);
    auto journeys = bfs.findPath(start, end, departureTime);

    if (journeys.empty()) {
        throw ContainerException("Маршрут не найден");
    }

    return {journeys[0]}; // Возвращаем только самый быстрый
}

std::vector<Journey> MinimalTransfersAlgorithm::findPath(const std::string& start, 
                                                         const std::string& end,
                                                         const Time& departureTime) {
    BFSAlgorithm bfs(system, 2);
    auto journeys = bfs.findPath(start, end, departureTime);

    if (journeys.empty()) {
        throw ContainerException("Маршрут не найден");
    }

    auto it = std::min_element(journeys.begin(), journeys.end(),
                               [](const Journey& a, const Journey& b) {
                                   return a.getTransferCount() < b.getTransferCount();
                               });

    return {*it};
}

void ArrivalTimeCalculationAlgorithm::calculateArrivalTimes(int tripId, double averageSpeed) {
    if (averageSpeed <= 0) {
        throw InputException("Средняя скорость должна быть положительной");
    }

    const auto& trips = system->getTrips();
    auto tripIt = std::find_if(trips.begin(), trips.end(),
                               [tripId](const auto& t) { return t->getTripId() == tripId; });

    if (tripIt == trips.end()) {
        throw ContainerException("Рейс с ID " + std::to_string(tripId) + " не найден");
    }

    auto trip = *tripIt;
    const auto& stopsList = trip->getRoute()->getAllStops();

    if (stopsList.empty()) {
        throw ContainerException("Маршрут не содержит остановок");
    }

    Time currentTime = trip->getStartTime();
    trip->setArrivalTime(stopsList[0], currentTime);

    const double distanceBetweenStops = 1.5; // км
    const int stopTime = 1; // минута

    for (size_t i = 1; i < stopsList.size(); ++i) {
        double travelTimeMinutes = (distanceBetweenStops / averageSpeed) * 60;
        Time arrivalTime = currentTime + static_cast<int>(travelTimeMinutes + 0.5);
        trip->setArrivalTime(stopsList[i], arrivalTime);
        currentTime = arrivalTime + stopTime;
    }
}

std::vector<std::shared_ptr<Route>> RouteSearchAlgorithm::findRoutes(const std::string& stopA, 
                                                                    const std::string& stopB) {
    std::vector<std::shared_ptr<Route>> foundRoutes;
    const auto& routes = system->getRoutes();
    
    for (const auto& route : routes) {
        if (route->containsStop(stopA) &&
            route->containsStop(stopB) &&
            route->isStopBefore(stopA, stopB)) {
            foundRoutes.push_back(route);
        }
    }
    return foundRoutes;
}

