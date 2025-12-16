#include "journey.h"

Journey::Journey(const std::vector<std::shared_ptr<Trip>>& tripList,
            const std::vector<std::string>& transfers,
            Time start, Time end)
    : trips(tripList), transferPoints(transfers),
      startTime(start), endTime(end),
      transferCount(static_cast<int>(transfers.size())) {}

int Journey::getTotalDuration() const {
    return endTime - startTime;
}

int Journey::getTransferCount() const {
    return transferCount;
}

Time Journey::getStartTime() const {
    return startTime;
}

Time Journey::getEndTime() const {
    return endTime;
}

const std::vector<std::shared_ptr<Trip>>& Journey::getTrips() const {
    return trips;
}

const std::vector<std::string>& Journey::getTransferPoints() const {
    return transferPoints;
}

void Journey::display() const {
    std::cout << "\nМаршрут поездки:\n";
    std::cout << "Общее время: " << getTotalDuration() << " минут\n";
    std::cout << "Пересадок: " << transferCount << "\n";
    std::cout << "Время отправления: " << startTime << "\n";
    std::cout << "Время прибытия: " << endTime << "\n";

    for (size_t i = 0; i < trips.size(); ++i) {
        std::cout << "\nЭтап " << (i + 1) << ":\n";
        std::cout << "  Маршрут: " << trips[i]->getRoute()->getNumber() << "\n";
        std::cout << "  Транспорт: " << trips[i]->getVehicle()->getInfo() << "\n";

        if (i > 0) {
            std::cout << "  Пересадка на: " << transferPoints[i - 1] << "\n";
        }
    }
}

