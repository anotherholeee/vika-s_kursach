#include <iostream>
#include <windows.h>
#include <limits>
#include "transport_system.h"
#include "ui.h"
#include "exceptions.h"
#include "bus.h"
#include "tram.h"
#include "trolleybus.h"
#include "driver.h"
#include "route.h"
#include "trip.h"
#include "time.h"
#include "stop.h"

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        TransportSystem system;

        system.loadData();

        // Если данных нет вообще (файлы не существуют или пустые), инициализируем тестовые данные
        // Но только если ВСЕ категории пустые, чтобы не добавлять дубликаты
        if (system.getStops().empty() && system.getDrivers().empty() && 
            system.getVehicles().empty() && system.getRoutes().empty() && 
            system.getTrips().empty()) {
            std::cout << "Инициализация тестовых данных...\n";
            initializeTestData(system);
            // Сохраняем сразу после инициализации
            system.saveData();
        }

        int choice;
        bool running = true;

        while (running) {
            displayLoginMenu();
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Неверный формат ввода. Введите число.\n";
                continue;
            }
            std::cin.ignore();

            switch (choice) {
                case 1: runAdminMode(system); break;
                case 2: runGuestMode(system); break;
                case 3:
                    system.saveData();
                    running = false;
                    std::cout << "Данные сохранены. Выход из программы.\n";
                    break;
                default: std::cout << "Неверный выбор.\n";
            }
        }
        
        system.saveData();

    } catch (const InputException& e) {
        std::cerr << "Критическая ошибка ввода: " << e.what() << std::endl;
        try {
            TransportSystem tempSystem;
            tempSystem.saveData();
        } catch (...) {
        }
        return 1;
    } catch (const FileException& e) {
        std::cerr << "Критическая ошибка файла: " << e.what() << std::endl;
        return 1;
    } catch (const ContainerException& e) {
        std::cerr << "Критическая ошибка данных: " << e.what() << std::endl;
        return 1;
    } catch (const TransportException& e) {
        std::cerr << "Критическая ошибка системы: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
