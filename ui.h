#ifndef UI_H
#define UI_H

#include "transport_system.h"
#include <string>

void displayGuestMenu();
void displayAdminMenu();
void displayLoginMenu();
void displayAllStopsForSelection(const TransportSystem& system);
std::string getStopNameByInput(const TransportSystem& system, const std::string& input);

void adminAddRoute(TransportSystem& system);
void adminAddTrip(TransportSystem& system);
void adminAddVehicle(TransportSystem& system);
void adminAddStop(TransportSystem& system);
void adminAddDriver(TransportSystem& system);

void viewTransportSchedule(TransportSystem& system);
void viewTransportScheduleGuest(TransportSystem& system);
void viewStopTimetable(TransportSystem& system);
void viewStopTimetableGuest(TransportSystem& system);
void viewStopTimetableAdmin(TransportSystem& system);
void searchRoutes(TransportSystem& system);
void calculateArrivalTime(TransportSystem& system);
void showAllTrips(const TransportSystem& system);

void runGuestMode(TransportSystem& system);
void runAdminMode(TransportSystem& system);

void initializeTestData(TransportSystem& system);

#endif // UI_H

