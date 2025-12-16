#include "ui.h"
#include "transport_system.h"
#include "exceptions.h"
#include "bus.h"
#include "tram.h"
#include "trolleybus.h"
#include "driver.h"
#include "route.h"
#include "trip.h"
#include "time.h"
#include "stop.h"
#include "journey.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <vector>
#include <string>
#include <set>

void displayGuestMenu() {
    std::cout << "\n=== ГОСТЕВОЙ РЕЖИМ ===\n";
    std::cout << "1. Просмотр расписания транспорта\n";
    std::cout << "2. Просмотр расписания остановки\n";
    std::cout << "3. Поиск маршрутов между остановками\n";
    std::cout << "0. Выход в главное меню\n";
    std::cout << "Выберите опцию: ";
}

void displayAdminMenu() {
    std::cout << "\n=== АДМИНИСТРАТИВНЫЙ РЕЖИМ ===\n";
    std::cout << "1. Просмотр расписания транспорта\n";
    std::cout << "2. Просмотр расписания остановки\n";
    std::cout << "3. Поиск маршрутов между остановками\n";
    std::cout << "4. Расчет времени прибытия для рейса\n";
    std::cout << "--- АДМИНИСТРАТИВНЫЕ ФУНКЦИИ ---\n";
    std::cout << "5. Добавить маршрут\n";
    std::cout << "6. Добавить рейс\n";
    std::cout << "7. Добавить транспортное средство\n";
    std::cout << "8. Добавить остановку\n";
    std::cout << "9. Добавить водителя\n";
    std::cout << "10. Удалить маршрут\n";
    std::cout << "11. Удалить рейс\n";
    std::cout << "12. Просмотр всех данных\n";
    std::cout << "13. Сохранить данные\n";
    std::cout << "14. Отменить последнее действие\n";
    std::cout << "15. Выход\n";
    std::cout << "Выберите опцию: ";
}

void displayLoginMenu() {
    std::cout << "\n=== СИСТЕМА РАСПИСАНИЯ ГОРОДСКОГО ТРАНСПОРТА ===\n";
    std::cout << "1. Войти как администратор\n";
    std::cout << "2. Войти как гость\n";
    std::cout << "3. Сохранить и выйти\n";
    std::cout << "Выберите опцию: ";
}

void displayAllStopsForSelection(const TransportSystem& system) {
    const auto& stops = system.getStops();
    std::cout << "\n=== СПИСОК ДОСТУПНЫХ ОСТАНОВКИ ===\n";
    for (const auto& stop : stops) {
        std::cout << "• " << stop.getName() << " (ID: " << stop.getId() << ")\n";
    }
    std::cout << "=================================\n";
}

std::string getStopNameByInput(const TransportSystem& system, const std::string& input) {
    const auto& stops = system.getStops();

    try {
        int id = std::stoi(input);
        for (const auto& stop : stops) {
            if (stop.getId() == id) {
                return stop.getName();
            }
        }
    } catch (...) {
    }

    for (const auto& stop : stops) {
        if (stop.getName() == input) {
            return stop.getName();
        }
    }

    throw ContainerException("Остановка не найдена: " + input);
}

void adminAddRoute(TransportSystem& system) {
    try {
        int number;
        std::string vehicleType;
        std::vector<std::string> stops;
        std::string stop;

        std::cout << "Введите номер маршрута: ";
        if (!(std::cin >> number)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для номера маршрута");
        }
        std::cin.ignore();

        std::cout << "Введите тип транспорта (Автобус/Трамвай/Троллейбус): ";
        std::getline(std::cin, vehicleType);

        std::cout << "Введите остановки маршрута (введите 'конец' для завершения):\n";
        while (true) {
            std::cout << "Остановка: ";
            std::getline(std::cin, stop);
            if (stop == "конец") break;
            stops.push_back(stop);
        }

        auto route = std::make_shared<Route>(number, vehicleType, stops);
        system.addRoute(route);
        std::cout << "Маршрут успешно добавлен!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddTrip(TransportSystem& system) {
    try {
        std::cout << "\n=== ДОСТУПНЫЕ МАРШРУТЫ ===\n";
        system.displayAllRoutes();
        std::cout << "==========================\n\n";

        std::cout << "=== ДОСТУПНЫЙ ТРАНСПОРТ ===\n";
        system.displayAllVehicles();
        std::cout << "===========================\n\n";

        const auto& drivers = system.getDrivers();
        if (drivers.empty()) {
            std::cout << "[DEBUG] Список водителей пуст. Количество водителей: " << drivers.size() << '\n';
            throw ContainerException("В системе нет водителей. Сначала добавьте водителя.");
        }

        std::cout << "=== ДОСТУПНЫЕ ВОДИТЕЛИ (всего: " << drivers.size() << ") ===\n";
        for (size_t i = 0; i < drivers.size(); ++i) {
            std::cout << (i+1) << ". " << drivers[i]->getFullName() 
                      << " (Имя: " << drivers[i]->getFirstName() 
                      << ", Фамилия: " << drivers[i]->getLastName();
            if (!drivers[i]->getMiddleName().empty()) {
                std::cout << ", Отчество: " << drivers[i]->getMiddleName();
            }
            std::cout << ")\n";
        }
        std::cout << "==========================\n\n";

        int tripId, routeNumber, driverChoice;
        std::string licensePlate, startTimeStr;

        std::cout << "Введите ID рейса: ";
        if (!(std::cin >> tripId)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для ID рейса");
        }
        std::cin.ignore();

        std::cout << "Введите номер маршрута: ";
        if (!(std::cin >> routeNumber)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для номера маршрута");
        }
        std::cin.ignore();

        auto route = system.findRouteByNumber(routeNumber);
        if (!route) {
            throw ContainerException("Маршрут не найден!");
        }

        std::cout << "Введите номерной знак транспортного средства: ";
        std::getline(std::cin, licensePlate);

        auto vehicle = system.findVehicleByLicensePlate(licensePlate);
        if (!vehicle) {
            throw ContainerException("Транспортное средство не найдено!");
        }

        std::cout << "Выберите водителя (введите номер из списка): ";
        if (!(std::cin >> driverChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для выбора водителя");
        }
        std::cin.ignore();

        if (driverChoice < 1 || driverChoice > static_cast<int>(drivers.size())) {
            throw InputException("Неверный выбор водителя. Допустимые значения: 1-" + 
                                std::to_string(drivers.size()));
        }

        auto driver = drivers[driverChoice - 1];

        std::cout << "Введите время отправления (HH:MM): ";
        std::getline(std::cin, startTimeStr);

        std::cout << "Введите день недели (1-понедельник, 2-вторник, ..., 7-воскресенье): ";
        int weekDay;
        if (!(std::cin >> weekDay)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для дня недели");
        }
        std::cin.ignore();
        
        if (weekDay < 1 || weekDay > 7) {
            throw InputException("День недели должен быть от 1 до 7");
        }

        Time startTime(startTimeStr);
        auto trip = std::make_shared<Trip>(tripId, route, vehicle, driver, startTime, weekDay);
        system.addTrip(trip);
        std::cout << "Рейс успешно добавлен!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddVehicle(TransportSystem& system) {
    try {
        std::string type, model, licensePlate;

        std::cout << "Введите тип транспорта (Автобус/Трамвай/Троллейбус): ";
        std::getline(std::cin, type);

        std::cout << "Введите модель: ";
        std::getline(std::cin, model);

        std::cout << "Введите номерной знак: ";
        std::getline(std::cin, licensePlate);

        std::shared_ptr<Vehicle> vehicle;
        if (type == "Автобус") {
            vehicle = std::make_shared<Bus>(model, licensePlate);
        } else if (type == "Трамвай") {
            vehicle = std::make_shared<Tram>(model, licensePlate);
        } else if (type == "Троллейбус") {
            vehicle = std::make_shared<Trolleybus>(model, licensePlate);
        }

        system.addVehicle(vehicle);
        std::cout << "Транспортное средство успешно добавлено!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddStop(TransportSystem& system) {
    try {
        int id;
        std::string name;

        std::cout << "Введите ID остановки: ";
        if (!(std::cin >> id)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для ID остановки");
        }
        std::cin.ignore();

        std::cout << "Введите название остановки: ";
        std::getline(std::cin, name);

        system.addStop(Stop(id, name));
        std::cout << "Остановка успешно добавлена!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddDriver(TransportSystem& system) {
    try {
        std::string firstName, lastName, middleName;

        std::cout << "Введите имя водителя: ";
        std::getline(std::cin, firstName);
        std::cout << "Введите фамилию водителя: ";
        std::getline(std::cin, lastName);
        std::cout << "Введите отчество водителя (если есть, иначе Enter): ";
        std::getline(std::cin, middleName);

        auto driver = std::make_shared<Driver>(firstName, lastName, middleName);
        system.addDriver(driver);
        std::cout << "Водитель успешно добавлен!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void initializeTestData(TransportSystem& system) {
    // Остановки (20 остановок)
    system.addStopDirect(Stop(1, "Центральный вокзал"));
    system.addStopDirect(Stop(2, "Площадь Ленина"));
    system.addStopDirect(Stop(3, "Улица Гагарина"));
    system.addStopDirect(Stop(4, "Парк Победы"));
    system.addStopDirect(Stop(5, "Стадион"));
    system.addStopDirect(Stop(6, "Больница"));
    system.addStopDirect(Stop(7, "Университет"));
    system.addStopDirect(Stop(8, "Торговый центр"));
    system.addStopDirect(Stop(9, "Железнодорожный вокзал"));
    system.addStopDirect(Stop(10, "Аэропорт"));
    system.addStopDirect(Stop(11, "Школа №1"));
    system.addStopDirect(Stop(12, "Поликлиника"));
    system.addStopDirect(Stop(13, "Центральный рынок"));
    system.addStopDirect(Stop(14, "Спортивный комплекс"));
    system.addStopDirect(Stop(15, "Театр"));
    system.addStopDirect(Stop(16, "Библиотека"));
    system.addStopDirect(Stop(17, "Парк культуры"));
    system.addStopDirect(Stop(18, "Автовокзал"));
    system.addStopDirect(Stop(19, "Завод"));
    system.addStopDirect(Stop(20, "Микрорайон Северный"));

    // Транспортные средства (15 единиц)
    auto bus1 = std::make_shared<Bus>("МАЗ-203", "АН 8669-7");
    auto bus2 = std::make_shared<Bus>("ПАЗ-3205", "ВС 1234-5");
    auto bus3 = std::make_shared<Bus>("ЛиАЗ-5292", "АВ 2222-2");
    auto bus4 = std::make_shared<Bus>("МАЗ-206", "АН 7777-7");
    auto bus5 = std::make_shared<Bus>("ПАЗ-4234", "ВС 8888-8");
    auto bus6 = std::make_shared<Bus>("ЛиАЗ-5256", "АВ 9999-9");
    
    auto tram1 = std::make_shared<Tram>("71-931", "ТР 5678-9");
    auto tram2 = std::make_shared<Tram>("71-623", "ТР 3333-3");
    auto tram3 = std::make_shared<Tram>("71-407", "ТР 4444-4");
    auto tram4 = std::make_shared<Tram>("71-931М", "ТР 5555-5");
    
    auto trolleybus1 = std::make_shared<Trolleybus>("ЗиУ-682", "ТБ 1111-1");
    auto trolleybus2 = std::make_shared<Trolleybus>("Тролза-5265", "ТБ 2222-2");
    auto trolleybus3 = std::make_shared<Trolleybus>("ЗиУ-683", "ТБ 3333-3");
    auto trolleybus4 = std::make_shared<Trolleybus>("Тролза-5275", "ТБ 4444-4");
    auto trolleybus5 = std::make_shared<Trolleybus>("ЗиУ-6205", "ТБ 5555-5");

    system.addVehicleDirect(bus1);
    system.addVehicleDirect(bus2);
    system.addVehicleDirect(bus3);
    system.addVehicleDirect(bus4);
    system.addVehicleDirect(bus5);
    system.addVehicleDirect(bus6);
    system.addVehicleDirect(tram1);
    system.addVehicleDirect(tram2);
    system.addVehicleDirect(tram3);
    system.addVehicleDirect(tram4);
    system.addVehicleDirect(trolleybus1);
    system.addVehicleDirect(trolleybus2);
    system.addVehicleDirect(trolleybus3);
    system.addVehicleDirect(trolleybus4);
    system.addVehicleDirect(trolleybus5);

    // Водители (15 человек)
    auto driver1 = std::make_shared<Driver>("Иван", "Петров", "Сергеевич");
    auto driver2 = std::make_shared<Driver>("Мария", "Сидорова", "Ивановна");
    auto driver3 = std::make_shared<Driver>("Алексей", "Козлов");
    auto driver4 = std::make_shared<Driver>("Дмитрий", "Смирнов", "Владимирович");
    auto driver5 = std::make_shared<Driver>("Елена", "Волкова");
    auto driver6 = std::make_shared<Driver>("Сергей", "Новиков", "Александрович");
    auto driver7 = std::make_shared<Driver>("Анна", "Морозова", "Дмитриевна");
    auto driver8 = std::make_shared<Driver>("Владимир", "Лебедев");
    auto driver9 = std::make_shared<Driver>("Ольга", "Соколова", "Андреевна");
    auto driver10 = std::make_shared<Driver>("Николай", "Попов", "Игоревич");
    auto driver11 = std::make_shared<Driver>("Татьяна", "Васильева");
    auto driver12 = std::make_shared<Driver>("Андрей", "Федоров", "Сергеевич");
    auto driver13 = std::make_shared<Driver>("Наталья", "Михайлова", "Владимировна");
    auto driver14 = std::make_shared<Driver>("Павел", "Кузнецов");
    auto driver15 = std::make_shared<Driver>("Юлия", "Орлова", "Александровна");

    system.addDriverDirect(driver1);
    system.addDriverDirect(driver2);
    system.addDriverDirect(driver3);
    system.addDriverDirect(driver4);
    system.addDriverDirect(driver5);
    system.addDriverDirect(driver6);
    system.addDriverDirect(driver7);
    system.addDriverDirect(driver8);
    system.addDriverDirect(driver9);
    system.addDriverDirect(driver10);
    system.addDriverDirect(driver11);
    system.addDriverDirect(driver12);
    system.addDriverDirect(driver13);
    system.addDriverDirect(driver14);
    system.addDriverDirect(driver15);

    // Маршруты (15 маршрутов)
    std::vector<std::string> route1Stops = {"Центральный вокзал", "Площадь Ленина", "Улица Гагарина", "Стадион"};
    auto route1 = std::make_shared<Route>(101, "Автобус", route1Stops);

    std::vector<std::string> route2Stops = {"Центральный вокзал", "Площадь Ленина", "Больница", "Университет"};
    auto route2 = std::make_shared<Route>(202, "Автобус", route2Stops);

    std::vector<std::string> route3Stops = {"Парк Победы", "Улица Гагарина", "Больница", "Университет"};
    auto route3 = std::make_shared<Route>(5, "Трамвай", route3Stops);

    std::vector<std::string> route4Stops = {"Стадион", "Улица Гагарина", "Парк Победы"};
    auto route4 = std::make_shared<Route>(303, "Троллейбус", route4Stops);

    std::vector<std::string> route5Stops = {"Университет", "Больница", "Площадь Ленина", "Центральный вокзал"};
    auto route5 = std::make_shared<Route>(404, "Автобус", route5Stops);

    std::vector<std::string> route6Stops = {"Парк Победы", "Улица Гагарина", "Площадь Ленина"};
    auto route6 = std::make_shared<Route>(7, "Трамвай", route6Stops);

    std::vector<std::string> route7Stops = {"Железнодорожный вокзал", "Центральный вокзал", "Торговый центр", "Поликлиника"};
    auto route7 = std::make_shared<Route>(105, "Автобус", route7Stops);

    std::vector<std::string> route8Stops = {"Аэропорт", "Железнодорожный вокзал", "Центральный вокзал", "Площадь Ленина"};
    auto route8 = std::make_shared<Route>(206, "Автобус", route8Stops);

    std::vector<std::string> route9Stops = {"Университет", "Школа №1", "Поликлиника", "Больница"};
    auto route9 = std::make_shared<Route>(8, "Трамвай", route9Stops);

    std::vector<std::string> route10Stops = {"Центральный рынок", "Площадь Ленина", "Театр", "Библиотека"};
    auto route10 = std::make_shared<Route>(304, "Троллейбус", route10Stops);

    std::vector<std::string> route11Stops = {"Спортивный комплекс", "Стадион", "Парк культуры", "Парк Победы"};
    auto route11 = std::make_shared<Route>(9, "Трамвай", route11Stops);

    std::vector<std::string> route12Stops = {"Автовокзал", "Центральный вокзал", "Площадь Ленина", "Торговый центр"};
    auto route12 = std::make_shared<Route>(107, "Автобус", route12Stops);

    std::vector<std::string> route13Stops = {"Завод", "Микрорайон Северный", "Университет", "Больница"};
    auto route13 = std::make_shared<Route>(305, "Троллейбус", route13Stops);

    std::vector<std::string> route14Stops = {"Парк культуры", "Библиотека", "Театр", "Центральный рынок"};
    auto route14 = std::make_shared<Route>(306, "Троллейбус", route14Stops);

    std::vector<std::string> route15Stops = {"Аэропорт", "Железнодорожный вокзал", "Автовокзал", "Центральный вокзал"};
    auto route15 = std::make_shared<Route>(108, "Автобус", route15Stops);

    system.addRouteDirect(route1);
    system.addRouteDirect(route2);
    system.addRouteDirect(route3);
    system.addRouteDirect(route4);
    system.addRouteDirect(route5);
    system.addRouteDirect(route6);
    system.addRouteDirect(route7);
    system.addRouteDirect(route8);
    system.addRouteDirect(route9);
    system.addRouteDirect(route10);
    system.addRouteDirect(route11);
    system.addRouteDirect(route12);
    system.addRouteDirect(route13);
    system.addRouteDirect(route14);
    system.addRouteDirect(route15);

    try {
        // Рейсы для будних дней (понедельник-пятница)
        // Маршрут 101
        auto trip1 = std::make_shared<Trip>(1, route1, bus1, driver1, Time("06:30"), 1);
        auto trip2 = std::make_shared<Trip>(2, route1, bus1, driver1, Time("08:00"), 1);
        auto trip3 = std::make_shared<Trip>(3, route1, bus2, driver2, Time("09:30"), 1);
        auto trip4 = std::make_shared<Trip>(4, route1, bus1, driver1, Time("11:00"), 1);
        auto trip5 = std::make_shared<Trip>(5, route1, bus3, driver3, Time("13:00"), 1);
        auto trip6 = std::make_shared<Trip>(6, route1, bus2, driver2, Time("15:00"), 1);
        auto trip7 = std::make_shared<Trip>(7, route1, bus1, driver1, Time("17:00"), 1);
        auto trip8 = std::make_shared<Trip>(8, route1, bus3, driver3, Time("19:00"), 1);
        
        // Маршрут 202
        auto trip9 = std::make_shared<Trip>(9, route2, bus2, driver2, Time("07:00"), 1);
        auto trip10 = std::make_shared<Trip>(10, route2, bus4, driver4, Time("08:30"), 1);
        auto trip11 = std::make_shared<Trip>(11, route2, bus2, driver2, Time("10:00"), 1);
        auto trip12 = std::make_shared<Trip>(12, route2, bus5, driver5, Time("12:00"), 1);
        auto trip13 = std::make_shared<Trip>(13, route2, bus4, driver4, Time("14:00"), 1);
        auto trip14 = std::make_shared<Trip>(14, route2, bus2, driver2, Time("16:00"), 1);
        auto trip15 = std::make_shared<Trip>(15, route2, bus5, driver5, Time("18:00"), 1);
        
        // Маршрут 5 (Трамвай)
        auto trip16 = std::make_shared<Trip>(16, route3, tram1, driver6, Time("06:00"), 1);
        auto trip17 = std::make_shared<Trip>(17, route3, tram2, driver7, Time("07:30"), 1);
        auto trip18 = std::make_shared<Trip>(18, route3, tram1, driver6, Time("09:00"), 1);
        auto trip19 = std::make_shared<Trip>(19, route3, tram3, driver8, Time("11:30"), 1);
        auto trip20 = std::make_shared<Trip>(20, route3, tram2, driver7, Time("13:30"), 1);
        auto trip21 = std::make_shared<Trip>(21, route3, tram1, driver6, Time("15:30"), 1);
        auto trip22 = std::make_shared<Trip>(22, route3, tram3, driver8, Time("17:30"), 1);
        auto trip23 = std::make_shared<Trip>(23, route3, tram4, driver9, Time("19:30"), 1);
        
        // Маршрут 303 (Троллейбус)
        auto trip24 = std::make_shared<Trip>(24, route4, trolleybus1, driver10, Time("07:15"), 1);
        auto trip25 = std::make_shared<Trip>(25, route4, trolleybus2, driver11, Time("09:15"), 1);
        auto trip26 = std::make_shared<Trip>(26, route4, trolleybus1, driver10, Time("11:15"), 1);
        auto trip27 = std::make_shared<Trip>(27, route4, trolleybus3, driver12, Time("13:15"), 1);
        auto trip28 = std::make_shared<Trip>(28, route4, trolleybus2, driver11, Time("15:15"), 1);
        auto trip29 = std::make_shared<Trip>(29, route4, trolleybus1, driver10, Time("17:15"), 1);
        
        // Маршрут 404
        auto trip30 = std::make_shared<Trip>(30, route5, bus3, driver13, Time("08:15"), 1);
        auto trip31 = std::make_shared<Trip>(31, route5, bus6, driver14, Time("10:15"), 1);
        auto trip32 = std::make_shared<Trip>(32, route5, bus3, driver13, Time("12:15"), 1);
        auto trip33 = std::make_shared<Trip>(33, route5, bus6, driver14, Time("14:15"), 1);
        auto trip34 = std::make_shared<Trip>(34, route5, bus3, driver13, Time("16:15"), 1);
        auto trip35 = std::make_shared<Trip>(35, route5, bus6, driver14, Time("18:15"), 1);
        
        // Маршрут 7 (Трамвай)
        auto trip36 = std::make_shared<Trip>(36, route6, tram2, driver15, Time("08:45"), 1);
        auto trip37 = std::make_shared<Trip>(37, route6, tram4, driver7, Time("10:45"), 1);
        auto trip38 = std::make_shared<Trip>(38, route6, tram2, driver15, Time("12:45"), 1);
        auto trip39 = std::make_shared<Trip>(39, route6, tram4, driver7, Time("14:45"), 1);
        auto trip40 = std::make_shared<Trip>(40, route6, tram2, driver15, Time("16:45"), 1);
        
        // Дополнительные маршруты
        auto trip41 = std::make_shared<Trip>(41, route7, bus4, driver1, Time("07:20"), 1);
        auto trip42 = std::make_shared<Trip>(42, route7, bus5, driver2, Time("09:20"), 1);
        auto trip43 = std::make_shared<Trip>(43, route7, bus4, driver1, Time("11:20"), 1);
        auto trip44 = std::make_shared<Trip>(44, route7, bus5, driver2, Time("13:20"), 1);
        auto trip45 = std::make_shared<Trip>(45, route7, bus4, driver1, Time("15:20"), 1);
        
        auto trip46 = std::make_shared<Trip>(46, route8, bus6, driver3, Time("06:45"), 1);
        auto trip47 = std::make_shared<Trip>(47, route8, bus1, driver4, Time("08:45"), 1);
        auto trip48 = std::make_shared<Trip>(48, route8, bus6, driver3, Time("10:45"), 1);
        auto trip49 = std::make_shared<Trip>(49, route8, bus1, driver4, Time("12:45"), 1);
        auto trip50 = std::make_shared<Trip>(50, route8, bus6, driver3, Time("14:45"), 1);
        
        auto trip51 = std::make_shared<Trip>(51, route9, tram1, driver5, Time("07:10"), 1);
        auto trip52 = std::make_shared<Trip>(52, route9, tram3, driver6, Time("09:10"), 1);
        auto trip53 = std::make_shared<Trip>(53, route9, tram1, driver5, Time("11:10"), 1);
        auto trip54 = std::make_shared<Trip>(54, route9, tram3, driver6, Time("13:10"), 1);
        auto trip55 = std::make_shared<Trip>(55, route9, tram1, driver5, Time("15:10"), 1);
        
        auto trip56 = std::make_shared<Trip>(56, route10, trolleybus2, driver7, Time("08:00"), 1);
        auto trip57 = std::make_shared<Trip>(57, route10, trolleybus4, driver8, Time("10:00"), 1);
        auto trip58 = std::make_shared<Trip>(58, route10, trolleybus2, driver7, Time("12:00"), 1);
        auto trip59 = std::make_shared<Trip>(59, route10, trolleybus4, driver8, Time("14:00"), 1);
        auto trip60 = std::make_shared<Trip>(60, route10, trolleybus2, driver7, Time("16:00"), 1);
        
        auto trip61 = std::make_shared<Trip>(61, route11, tram2, driver9, Time("08:30"), 1);
        auto trip62 = std::make_shared<Trip>(62, route11, tram4, driver10, Time("10:30"), 1);
        auto trip63 = std::make_shared<Trip>(63, route11, tram2, driver9, Time("12:30"), 1);
        auto trip64 = std::make_shared<Trip>(64, route11, tram4, driver10, Time("14:30"), 1);
        
        auto trip65 = std::make_shared<Trip>(65, route12, bus2, driver11, Time("07:40"), 1);
        auto trip66 = std::make_shared<Trip>(66, route12, bus3, driver12, Time("09:40"), 1);
        auto trip67 = std::make_shared<Trip>(67, route12, bus2, driver11, Time("11:40"), 1);
        auto trip68 = std::make_shared<Trip>(68, route12, bus3, driver12, Time("13:40"), 1);
        auto trip69 = std::make_shared<Trip>(69, route12, bus2, driver11, Time("15:40"), 1);
        
        auto trip70 = std::make_shared<Trip>(70, route13, trolleybus3, driver13, Time("08:20"), 1);
        auto trip71 = std::make_shared<Trip>(71, route13, trolleybus5, driver14, Time("10:20"), 1);
        auto trip72 = std::make_shared<Trip>(72, route13, trolleybus3, driver13, Time("12:20"), 1);
        auto trip73 = std::make_shared<Trip>(73, route13, trolleybus5, driver14, Time("14:20"), 1);
        
        auto trip74 = std::make_shared<Trip>(74, route14, trolleybus1, driver15, Time("09:00"), 1);
        auto trip75 = std::make_shared<Trip>(75, route14, trolleybus4, driver1, Time("11:00"), 1);
        auto trip76 = std::make_shared<Trip>(76, route14, trolleybus1, driver15, Time("13:00"), 1);
        auto trip77 = std::make_shared<Trip>(77, route14, trolleybus4, driver1, Time("15:00"), 1);
        
        auto trip78 = std::make_shared<Trip>(78, route15, bus5, driver2, Time("07:50"), 1);
        auto trip79 = std::make_shared<Trip>(79, route15, bus6, driver3, Time("09:50"), 1);
        auto trip80 = std::make_shared<Trip>(80, route15, bus5, driver2, Time("11:50"), 1);
        auto trip81 = std::make_shared<Trip>(81, route15, bus6, driver3, Time("13:50"), 1);
        auto trip82 = std::make_shared<Trip>(82, route15, bus5, driver2, Time("15:50"), 1);
        
        // Рейсы для выходных дней (суббота, воскресенье)
        auto trip83 = std::make_shared<Trip>(83, route1, bus1, driver1, Time("08:00"), 6);
        auto trip84 = std::make_shared<Trip>(84, route1, bus2, driver2, Time("10:00"), 6);
        auto trip85 = std::make_shared<Trip>(85, route1, bus3, driver3, Time("12:00"), 6);
        auto trip86 = std::make_shared<Trip>(86, route1, bus1, driver1, Time("14:00"), 6);
        auto trip87 = std::make_shared<Trip>(87, route1, bus2, driver2, Time("16:00"), 6);
        
        auto trip88 = std::make_shared<Trip>(88, route2, bus2, driver2, Time("09:00"), 6);
        auto trip89 = std::make_shared<Trip>(89, route2, bus4, driver4, Time("11:00"), 6);
        auto trip90 = std::make_shared<Trip>(90, route2, bus5, driver5, Time("13:00"), 6);
        auto trip91 = std::make_shared<Trip>(91, route2, bus2, driver2, Time("15:00"), 6);
        
        auto trip92 = std::make_shared<Trip>(92, route3, tram1, driver6, Time("08:30"), 6);
        auto trip93 = std::make_shared<Trip>(93, route3, tram2, driver7, Time("10:30"), 6);
        auto trip94 = std::make_shared<Trip>(94, route3, tram3, driver8, Time("12:30"), 6);
        auto trip95 = std::make_shared<Trip>(95, route3, tram1, driver6, Time("14:30"), 6);
        
        auto trip96 = std::make_shared<Trip>(96, route4, trolleybus1, driver10, Time("09:15"), 6);
        auto trip97 = std::make_shared<Trip>(97, route4, trolleybus2, driver11, Time("11:15"), 6);
        auto trip98 = std::make_shared<Trip>(98, route4, trolleybus3, driver12, Time("13:15"), 6);
        auto trip99 = std::make_shared<Trip>(99, route4, trolleybus1, driver10, Time("15:15"), 6);
        
        auto trip100 = std::make_shared<Trip>(100, route1, bus1, driver1, Time("09:00"), 7);
        auto trip101 = std::make_shared<Trip>(101, route1, bus2, driver2, Time("11:00"), 7);
        auto trip102 = std::make_shared<Trip>(102, route1, bus3, driver3, Time("13:00"), 7);
        auto trip103 = std::make_shared<Trip>(103, route1, bus1, driver1, Time("15:00"), 7);
        
        auto trip104 = std::make_shared<Trip>(104, route2, bus2, driver2, Time("10:00"), 7);
        auto trip105 = std::make_shared<Trip>(105, route2, bus4, driver4, Time("12:00"), 7);
        auto trip106 = std::make_shared<Trip>(106, route2, bus5, driver5, Time("14:00"), 7);
        
        auto trip107 = std::make_shared<Trip>(107, route3, tram1, driver6, Time("09:30"), 7);
        auto trip108 = std::make_shared<Trip>(108, route3, tram2, driver7, Time("11:30"), 7);
        auto trip109 = std::make_shared<Trip>(109, route3, tram3, driver8, Time("13:30"), 7);
        
        auto trip110 = std::make_shared<Trip>(110, route4, trolleybus1, driver10, Time("10:15"), 7);
        auto trip111 = std::make_shared<Trip>(111, route4, trolleybus2, driver11, Time("12:15"), 7);
        auto trip112 = std::make_shared<Trip>(112, route4, trolleybus3, driver12, Time("14:15"), 7);

        // Добавляем все рейсы в систему
        system.addTripDirect(trip1);
        system.addTripDirect(trip2);
        system.addTripDirect(trip3);
        system.addTripDirect(trip4);
        system.addTripDirect(trip5);
        system.addTripDirect(trip6);
        system.addTripDirect(trip7);
        system.addTripDirect(trip8);
        system.addTripDirect(trip9);
        system.addTripDirect(trip10);
        system.addTripDirect(trip11);
        system.addTripDirect(trip12);
        system.addTripDirect(trip13);
        system.addTripDirect(trip14);
        system.addTripDirect(trip15);
        system.addTripDirect(trip16);
        system.addTripDirect(trip17);
        system.addTripDirect(trip18);
        system.addTripDirect(trip19);
        system.addTripDirect(trip20);
        system.addTripDirect(trip21);
        system.addTripDirect(trip22);
        system.addTripDirect(trip23);
        system.addTripDirect(trip24);
        system.addTripDirect(trip25);
        system.addTripDirect(trip26);
        system.addTripDirect(trip27);
        system.addTripDirect(trip28);
        system.addTripDirect(trip29);
        system.addTripDirect(trip30);
        system.addTripDirect(trip31);
        system.addTripDirect(trip32);
        system.addTripDirect(trip33);
        system.addTripDirect(trip34);
        system.addTripDirect(trip35);
        system.addTripDirect(trip36);
        system.addTripDirect(trip37);
        system.addTripDirect(trip38);
        system.addTripDirect(trip39);
        system.addTripDirect(trip40);
        system.addTripDirect(trip41);
        system.addTripDirect(trip42);
        system.addTripDirect(trip43);
        system.addTripDirect(trip44);
        system.addTripDirect(trip45);
        system.addTripDirect(trip46);
        system.addTripDirect(trip47);
        system.addTripDirect(trip48);
        system.addTripDirect(trip49);
        system.addTripDirect(trip50);
        system.addTripDirect(trip51);
        system.addTripDirect(trip52);
        system.addTripDirect(trip53);
        system.addTripDirect(trip54);
        system.addTripDirect(trip55);
        system.addTripDirect(trip56);
        system.addTripDirect(trip57);
        system.addTripDirect(trip58);
        system.addTripDirect(trip59);
        system.addTripDirect(trip60);
        system.addTripDirect(trip61);
        system.addTripDirect(trip62);
        system.addTripDirect(trip63);
        system.addTripDirect(trip64);
        system.addTripDirect(trip65);
        system.addTripDirect(trip66);
        system.addTripDirect(trip67);
        system.addTripDirect(trip68);
        system.addTripDirect(trip69);
        system.addTripDirect(trip70);
        system.addTripDirect(trip71);
        system.addTripDirect(trip72);
        system.addTripDirect(trip73);
        system.addTripDirect(trip74);
        system.addTripDirect(trip75);
        system.addTripDirect(trip76);
        system.addTripDirect(trip77);
        system.addTripDirect(trip78);
        system.addTripDirect(trip79);
        system.addTripDirect(trip80);
        system.addTripDirect(trip81);
        system.addTripDirect(trip82);
        system.addTripDirect(trip83);
        system.addTripDirect(trip84);
        system.addTripDirect(trip85);
        system.addTripDirect(trip86);
        system.addTripDirect(trip87);
        system.addTripDirect(trip88);
        system.addTripDirect(trip89);
        system.addTripDirect(trip90);
        system.addTripDirect(trip91);
        system.addTripDirect(trip92);
        system.addTripDirect(trip93);
        system.addTripDirect(trip94);
        system.addTripDirect(trip95);
        system.addTripDirect(trip96);
        system.addTripDirect(trip97);
        system.addTripDirect(trip98);
        system.addTripDirect(trip99);
        system.addTripDirect(trip100);
        system.addTripDirect(trip101);
        system.addTripDirect(trip102);
        system.addTripDirect(trip103);
        system.addTripDirect(trip104);
        system.addTripDirect(trip105);
        system.addTripDirect(trip106);
        system.addTripDirect(trip107);
        system.addTripDirect(trip108);
        system.addTripDirect(trip109);
        system.addTripDirect(trip110);
        system.addTripDirect(trip111);
        system.addTripDirect(trip112);

        // Рассчитываем время прибытия для всех рейсов
        // Скорости: автобусы ~30 км/ч, трамваи ~25 км/ч, троллейбусы ~28 км/ч
        for (int i = 1; i <= 112; i++) {
            auto trip = system.getTripById(i);
            if (trip) {
                std::string vehicleType = trip->getRoute()->getVehicleType();
                double speed = 30.0; // по умолчанию
                if (vehicleType == "Трамвай") speed = 25.0;
                else if (vehicleType == "Троллейбус") speed = 28.0;
                system.calculateArrivalTimes(i, speed);
            }
        }

    } catch (const std::exception& e) {
        std::cout << "Ошибка при создании тестовых рейсов: " << e.what() << "\n";
    }
}

void searchRoutes(TransportSystem& system) {
    try {
        std::string stopAInput, stopBInput;

        displayAllStopsForSelection(system);

        std::cout << "\nВведите начальную остановку (ID или название): ";
        std::getline(std::cin, stopAInput);
        std::cout << "Введите конечную остановку (ID или название): ";
        std::getline(std::cin, stopBInput);

        std::string stopA = getStopNameByInput(system, stopAInput);
        std::string stopB = getStopNameByInput(system, stopBInput);

        auto routes = system.findRoutes(stopA, stopB);
        std::cout << "\nНайдено прямых маршрутов: " << routes.size() << '\n';

        if (!routes.empty()) {
            for (const auto& route : routes) {
                std::cout << "\nМаршрут " << route->getNumber() << " ("
                          << route->getVehicleType() << ")\n";

                const auto& allStops = route->getAllStops();
                int startPos = route->getStopPosition(stopA);
                int endPos = route->getStopPosition(stopB);

                if (startPos != -1 && endPos != -1 && startPos < endPos) {
                    std::cout << "Путь: " << allStops[startPos];
                    for (int i = startPos + 1; i <= endPos; i++) {
                        std::cout << " → " << allStops[i];
                    }
                    std::cout << '\n';
                }
            }
        }

        // Если прямых маршрутов нет, автоматически ищем маршруты с пересадками
        if (routes.empty()) {
            std::cout << "\nПрямых маршрутов не найдено. Ищем маршруты с пересадками...\n";
            
            try {
                auto& planner = system.getJourneyPlanner();
                auto journeys = planner.findAllJourneysWithTransfers(stopA, stopB, 2);
                
                if (journeys.empty()) {
                    std::cout << "Маршрутов с пересадками не найдено.\n";
                } else {
                    // Фильтруем уникальные варианты по комбинации маршрутов и точек пересадки
                    std::vector<Journey> uniqueJourneys;
                    std::set<std::string> seenRoutes;
                    
                    for (const auto& journey : journeys) {
                        // Создаем уникальный ключ: последовательность номеров маршрутов + точки пересадки
                        std::string routeKey;
                        const auto& trips = journey.getTrips();
                        const auto& transferPoints = journey.getTransferPoints();
                        
                        for (size_t i = 0; i < trips.size(); ++i) {
                            routeKey += std::to_string(trips[i]->getRoute()->getNumber());
                            if (i < transferPoints.size()) {
                                routeKey += "@" + transferPoints[i] + "@";
                            }
                        }
                        
                        // Добавляем только если такой комбинации еще не было
                        if (seenRoutes.find(routeKey) == seenRoutes.end()) {
                            seenRoutes.insert(routeKey);
                            uniqueJourneys.push_back(journey);
                        }
                    }
                    
                    if (uniqueJourneys.empty()) {
                        std::cout << "Маршрутов с пересадками не найдено.\n";
                    } else {
                        std::cout << "\n========================================\n";
                        
                        // Показываем до 5 уникальных вариантов
                        int count = std::min(5, static_cast<int>(uniqueJourneys.size()));
                        for (int i = 0; i < count; ++i) {
                            std::cout << "\n--- Вариант " << (i + 1) << " ---\n";
                            const auto& journey = uniqueJourneys[i];
                            
                            std::cout << "Пересадок: " << journey.getTransferCount() << "\n";
                            std::cout << "Общее время в пути: " << journey.getTotalDuration() << " минут\n";
                            std::cout << "Время отправления: " << journey.getStartTime() << "\n";
                            std::cout << "Время прибытия: " << journey.getEndTime() << "\n";
                            
                            const auto& trips = journey.getTrips();
                            const auto& transferPoints = journey.getTransferPoints();
                            
                            std::cout << "\nПуть:\n";
                            std::cout << "  " << stopA;
                            
                            for (size_t j = 0; j < trips.size(); ++j) {
                                const auto& trip = trips[j];
                                const auto& route = trip->getRoute();
                                const auto& routeStops = route->getAllStops();
                                
                                // Определяем начальную и конечную остановки для этого участка
                                std::string segmentStart = (j == 0) ? stopA : transferPoints[j - 1];
                                std::string segmentEnd = (j < transferPoints.size()) ? transferPoints[j] : stopB;
                                
                                int startPos = route->getStopPosition(segmentStart);
                                int endPos = route->getStopPosition(segmentEnd);
                                
                                if (startPos != -1 && endPos != -1 && startPos < endPos) {
                                    for (int k = startPos + 1; k <= endPos; ++k) {
                                        std::cout << " → " << routeStops[k];
                                    }
                                }
                                
                                std::cout << " [Маршрут " << route->getNumber() 
                                          << " (" << route->getVehicleType() << ")]";
                                
                                if (j < transferPoints.size()) {
                                    std::cout << "\n  Пересадка на остановке: " << transferPoints[j];
                                    std::cout << "\n  " << transferPoints[j];
                                }
                            }
                            std::cout << "\n";
                        }
                        std::cout << "========================================\n";
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "Ошибка при поиске маршрутов с пересадками: " << e.what() << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void viewStopTimetableGuest(TransportSystem& system) {
    try {
        displayAllStopsForSelection(system);

        std::string stopInput;
        std::cout << "\nВведите остановку (ID или название): ";
        std::getline(std::cin, stopInput);

        std::string stopName = getStopNameByInput(system, stopInput);
        system.getStopTimetableAll(stopName);
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void viewStopTimetableAdmin(TransportSystem& system) {
    try {
        displayAllStopsForSelection(system);

        std::string stopInput;
        std::cout << "\nВведите остановку (ID или название): ";
        std::getline(std::cin, stopInput);

        std::string stopName = getStopNameByInput(system, stopInput);
        
        const auto& trips = system.getTrips();
        std::vector<std::pair<int, std::pair<int, Time>>> relevantTrips; // tripId, routeNumber, time

        for (const auto& trip : trips) {
            if (trip->hasStop(stopName)) {
                Time arrivalTime = trip->getArrivalTime(stopName);
                relevantTrips.push_back({trip->getTripId(), {trip->getRoute()->getNumber(), arrivalTime}});
            }
        }

        std::sort(relevantTrips.begin(), relevantTrips.end(),
                  [](const auto& a, const auto& b) { return a.second.second < b.second.second; });

        std::cout << "\nРасписание для остановки '" << stopName << "':\n";
        if (relevantTrips.empty()) {
            std::cout << "Рейсов не найдено.\n";
        } else {
            for (const auto& [tripId, routeTime] : relevantTrips) {
                auto tripIt = std::find_if(trips.begin(), trips.end(),
                                          [tripId](const auto& t) { return t->getTripId() == tripId; });
                if (tripIt != trips.end()) {
                    auto trip = *tripIt;
                    std::cout << trip->getRoute()->getVehicleType() << " " << routeTime.first << " - прибытие в " << routeTime.second
                              << " | Транспорт: " << trip->getVehicle()->getInfo()
                              << " | Водитель: " << trip->getDriver()->getFullName() << '\n';
                } else {
                    std::cout << "Маршрут " << routeTime.first << " - прибытие в " << routeTime.second << '\n';
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void viewStopTimetable(TransportSystem& system) {
    viewStopTimetableAdmin(system);
}

void viewTransportScheduleGuest(TransportSystem& system) {
    try {
        std::cout << "\n=== ВЫБОР ТИПА ТРАНСПОРТА ===\n";
        std::cout << "1. Автобус\n";
        std::cout << "2. Троллейбус\n";
        std::cout << "3. Трамвай\n";
        std::cout << "Выберите тип транспорта: ";

        int transportTypeChoice;
        if (!(std::cin >> transportTypeChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для типа транспорта");
        }
        std::cin.ignore();

        std::string selectedType;
        switch (transportTypeChoice) {
            case 1: selectedType = "Автобус"; break;
            case 2: selectedType = "Троллейбус"; break;
            case 3: selectedType = "Трамвай"; break;
            default:
                throw InputException("Неверный выбор типа транспорта. Допустимые значения: 1-3");
        }

        // Получаем маршруты выбранного типа
        const auto& routes = system.getRoutes();
        std::vector<std::shared_ptr<Route>> filteredRoutes;
        for (const auto& route : routes) {
            if (route->getVehicleType() == selectedType) {
                filteredRoutes.push_back(route);
            }
        }

        if (filteredRoutes.empty()) {
            std::cout << "\nМаршрутов типа '" << selectedType << "' не найдено.\n";
            return;
        }

        // Показываем список маршрутов
        std::cout << "\n=== ДОСТУПНЫЕ МАРШРУТЫ (" << selectedType << ") ===\n";
        for (size_t i = 0; i < filteredRoutes.size(); ++i) {
            std::cout << (i + 1) << ". Маршрут " << filteredRoutes[i]->getNumber()
                      << ": " << filteredRoutes[i]->getStartStop()
                      << " → " << filteredRoutes[i]->getEndStop() << "\n";
        }
        std::cout << "0. Назад\n";
        std::cout << "Выберите маршрут: ";

        int routeChoice;
        if (!(std::cin >> routeChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для выбора маршрута");
        }
        std::cin.ignore();

        if (routeChoice == 0) {
            return;
        }

        if (routeChoice < 1 || routeChoice > static_cast<int>(filteredRoutes.size())) {
            throw InputException("Неверный выбор маршрута. Допустимые значения: 0-" +
                                std::to_string(filteredRoutes.size()));
        }

        auto selectedRoute = filteredRoutes[routeChoice - 1];

        // Выбор дня недели
        std::cout << "\n=== ВЫБОР ДНЯ НЕДЕЛИ ===\n";
        std::cout << "1. Понедельник\n";
        std::cout << "2. Вторник\n";
        std::cout << "3. Среда\n";
        std::cout << "4. Четверг\n";
        std::cout << "5. Пятница\n";
        std::cout << "6. Суббота\n";
        std::cout << "7. Воскресенье\n";
        std::cout << "0. Назад\n";
        std::cout << "Выберите день недели: ";

        int weekDayChoice;
        if (!(std::cin >> weekDayChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для дня недели");
        }
        std::cin.ignore();

        if (weekDayChoice == 0) {
            return;
        }

        if (weekDayChoice < 1 || weekDayChoice > 7) {
            throw InputException("Неверный выбор дня недели. Допустимые значения: 0-7");
        }

        // Получаем рейсы для выбранного маршрута и дня недели
        const auto& trips = system.getTrips();
        std::vector<std::shared_ptr<Trip>> filteredTrips;
        
        for (const auto& trip : trips) {
            if (trip->getRoute()->getNumber() == selectedRoute->getNumber() &&
                trip->getRoute()->getVehicleType() == selectedType) {
                int tripDay = trip->getWeekDay();
                bool dayMatches = false;
                if (weekDayChoice >= 1 && weekDayChoice <= 5) {
                    // Будни: показываем все будни (1-5)
                    dayMatches = (tripDay >= 1 && tripDay <= 5);
                } else {
                    // Выходные: показываем конкретный день
                    dayMatches = (tripDay == weekDayChoice);
                }
                
                if (dayMatches) {
                    filteredTrips.push_back(trip);
                }
            }
        }

        if (filteredTrips.empty()) {
            std::string dayNames[] = {"", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"};
            std::cout << "\nРейсов для маршрута " << selectedRoute->getNumber() 
                      << " на " << dayNames[weekDayChoice] << " не найдено.\n";
            return;
        }

        // Сортируем рейсы по времени отправления
        std::sort(filteredTrips.begin(), filteredTrips.end(),
                  [](const auto& a, const auto& b) { return a->getStartTime() < b->getStartTime(); });

        std::string dayNames[] = {"", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"};
        
        std::cout << "\n=== РАСПИСАНИЕ МАРШРУТА " << selectedRoute->getNumber() << " ===\n";
        std::cout << "Тип транспорта: " << selectedType << "\n";
        std::cout << "День недели: " << dayNames[weekDayChoice] << "\n";
        std::cout << "Маршрут: " << selectedRoute->getStartStop() 
                  << " → " << selectedRoute->getEndStop() << "\n";
        std::cout << "========================================\n";

        for (const auto& trip : filteredTrips) {
            std::cout << "\nРейс " << trip->getTripId() << " (отправление: " << trip->getStartTime() << "):\n";
            
            const auto& schedule = trip->getSchedule();
            if (!schedule.empty()) {
                // Преобразуем map в vector и сортируем по времени
                std::vector<std::pair<std::string, Time>> sortedSchedule(schedule.begin(), schedule.end());
                std::sort(sortedSchedule.begin(), sortedSchedule.end(),
                          [](const auto& a, const auto& b) { return a.second < b.second; });
                for (const auto& [stop, time] : sortedSchedule) {
                    std::cout << "  " << stop << " - " << time << "\n";
                }
            } else {
                std::cout << "  Расписание: не рассчитано\n";
            }
            std::cout << "  ---\n";
        }
        std::cout << "========================================\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void viewTransportSchedule(TransportSystem& system) {
    try {
        std::cout << "\n=== ВЫБОР ТИПА ТРАНСПОРТА ===\n";
        std::cout << "1. Автобус\n";
        std::cout << "2. Троллейбус\n";
        std::cout << "3. Трамвай\n";
        std::cout << "Выберите тип транспорта: ";

        int transportTypeChoice;
        if (!(std::cin >> transportTypeChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для типа транспорта");
        }
        std::cin.ignore();

        std::string selectedType;
        switch (transportTypeChoice) {
            case 1: selectedType = "Автобус"; break;
            case 2: selectedType = "Троллейбус"; break;
            case 3: selectedType = "Трамвай"; break;
            default:
                throw InputException("Неверный выбор типа транспорта. Допустимые значения: 1-3");
        }

        const auto& routes = system.getRoutes();
        std::vector<std::shared_ptr<Route>> filteredRoutes;
        for (const auto& route : routes) {
            if (route->getVehicleType() == selectedType) {
                filteredRoutes.push_back(route);
            }
        }

        if (filteredRoutes.empty()) {
            std::cout << "Маршрутов типа '" << selectedType << "' не найдено.\n";
            return;
        }

        std::cout << "\n=== ДОСТУПНЫЕ МАРШРУТЫ (" << selectedType << ") ===\n";
        for (size_t i = 0; i < filteredRoutes.size(); ++i) {
            std::cout << (i + 1) << ". Маршрут " << filteredRoutes[i]->getNumber()
                      << ": " << filteredRoutes[i]->getStartStop()
                      << " → " << filteredRoutes[i]->getEndStop() << "\n";
        }
        std::cout << "0. Назад\n";
        std::cout << "Выберите маршрут: ";

        int routeChoice;
        if (!(std::cin >> routeChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для выбора маршрута");
        }
        std::cin.ignore();

        if (routeChoice == 0) {
            return;
        }

        if (routeChoice < 1 || routeChoice > static_cast<int>(filteredRoutes.size())) {
            throw InputException("Неверный выбор маршрута. Допустимые значения: 0-" +
                                std::to_string(filteredRoutes.size()));
        }

        auto selectedRoute = filteredRoutes[routeChoice - 1];

        std::cout << "\n=== ВЫБОР НАПРАВЛЕНИЯ ===\n";
        std::cout << "1. Прямой (" << selectedRoute->getStartStop()
                  << " → " << selectedRoute->getEndStop() << ")\n";
        std::cout << "2. Обратный (" << selectedRoute->getEndStop()
                  << " → " << selectedRoute->getStartStop() << ")\n";
        std::cout << "0. Назад\n";
        std::cout << "Выберите направление: ";

        int directionChoice;
        if (!(std::cin >> directionChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для выбора направления");
        }
        std::cin.ignore();

        if (directionChoice == 0) {
            return;
        }

        if (directionChoice != 1 && directionChoice != 2) {
            throw InputException("Неверный выбор направления. Допустимые значения: 0, 1 или 2");
        }

        std::vector<std::string> routeStops;
        if (directionChoice == 1) {
            routeStops = selectedRoute->getAllStops();
        } else {
            routeStops = selectedRoute->getAllStops();
            std::reverse(routeStops.begin(), routeStops.end());
        }

        std::cout << "\n=== ОСТАНОВКИ МАРШРУТА ===\n";
        for (size_t i = 0; i < routeStops.size(); ++i) {
            std::cout << (i + 1) << ". " << routeStops[i] << "\n";
        }
        std::cout << "0. Назад\n";
        std::cout << "Выберите остановку: ";

        int stopChoice;
        if (!(std::cin >> stopChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для выбора остановки");
        }
        std::cin.ignore();

        if (stopChoice == 0) {
            return;
        }

        if (stopChoice < 1 || stopChoice > static_cast<int>(routeStops.size())) {
            throw InputException("Неверный выбор остановки. Допустимые значения: 0-" +
                                std::to_string(routeStops.size()));
        }

        std::string selectedStop = routeStops[stopChoice - 1];

        std::cout << "\n=== ВЫБОР ДНЯ НЕДЕЛИ ===\n";
        std::cout << "1. Понедельник\n";
        std::cout << "2. Вторник\n";
        std::cout << "3. Среда\n";
        std::cout << "4. Четверг\n";
        std::cout << "5. Пятница\n";
        std::cout << "6. Суббота\n";
        std::cout << "7. Воскресенье\n";
        std::cout << "0. Назад\n";
        std::cout << "Выберите день недели (0-7): ";

        int weekDayChoice;
        if (!(std::cin >> weekDayChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для дня недели");
        }
        std::cin.ignore();

        if (weekDayChoice == 0) {
            return;
        }

        if (weekDayChoice < 1 || weekDayChoice > 7) {
            throw InputException("Неверный выбор дня недели. Допустимые значения: 0-7");
        }

        const auto& trips = system.getTrips();
        std::vector<std::pair<int, Time>> stopTimes;

        for (const auto& trip : trips) {
            if (trip->getRoute()->getNumber() == selectedRoute->getNumber() &&
                trip->getRoute()->getVehicleType() == selectedType) {
                int tripDay = trip->getWeekDay();
                bool dayMatches = false;
                if (weekDayChoice >= 1 && weekDayChoice <= 5) {
                    dayMatches = (tripDay >= 1 && tripDay <= 5);
                } else {
                    dayMatches = (tripDay == weekDayChoice);
                }
                
                if (dayMatches && trip->hasStop(selectedStop)) {
                    Time arrivalTime = trip->getArrivalTime(selectedStop);
                    stopTimes.push_back({trip->getTripId(), arrivalTime});
                }
            }
        }

        std::sort(stopTimes.begin(), stopTimes.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });

        std::string dayNames[] = {"", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"};
        std::cout << "\n=== РАСПИСАНИЕ ДЛЯ ОСТАНОВКИ '" << selectedStop << "' ===\n";
        std::cout << "Маршрут: " << selectedRoute->getNumber() << " (" << selectedType << ")\n";
        std::cout << "Направление: " << (directionChoice == 1 ? "Прямое" : "Обратное") << "\n";
        std::cout << "День недели: " << dayNames[weekDayChoice] << "\n";
        std::cout << "----------------------------------------\n";

        if (stopTimes.empty()) {
            std::cout << "Рейсов не найдено для этой остановки.\n";
        } else {
            for (const auto& [tripId, time] : stopTimes) {
                // Находим рейс для вывода дополнительной информации
                auto tripIt = std::find_if(trips.begin(), trips.end(),
                                          [tripId](const auto& t) { return t->getTripId() == tripId; });
                if (tripIt != trips.end()) {
                    auto trip = *tripIt;
                    std::cout << "Рейс " << tripId << " - прибытие в " << time
                              << " | Транспорт: " << trip->getVehicle()->getInfo()
                              << " | Водитель: " << trip->getDriver()->getFullName() << "\n";
                } else {
                    std::cout << "Рейс " << tripId << " - прибытие в " << time << "\n";
                }
            }
        }
        std::cout << "========================================\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void calculateArrivalTime(TransportSystem& system) {
    try {
        const auto& trips = system.getTrips();
        if (trips.empty()) {
            std::cout << "В системе нет рейсов.\n";
            return;
        }

        std::cout << "\n=== ДОСТУПНЫЕ РЕЙСЫ ===\n";
        for (const auto& trip : trips) {
            std::cout << "ID: " << trip->getTripId()
                      << " | Маршрут: " << trip->getRoute()->getNumber()
                      << " | Отправление: " << trip->getStartTime()
                      << " | Направление: " << trip->getRoute()->getStartStop()
                      << " → " << trip->getRoute()->getEndStop() << '\n';
        }
        std::cout << "========================\n\n";

        int tripId;
        double speed;

        std::cout << "Введите ID рейса: ";
        if (!(std::cin >> tripId)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для ID рейса");
        }
        std::cout << "Введите среднюю скорость (км/ч): ";
        if (!(std::cin >> speed)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputException("Неверный формат ввода для скорости");
        }
        std::cin.ignore();

        system.calculateArrivalTimes(tripId, speed);

        auto tripIt = std::find_if(trips.begin(), trips.end(),
                                  [tripId](const auto& t) { return t->getTripId() == tripId; });
        if (tripIt != trips.end()) {
            auto trip = *tripIt;
            std::cout << "\nОбновленное расписание для рейса " << tripId << ":\n";
            const auto& schedule = trip->getSchedule();
            // Преобразуем map в vector и сортируем по времени
            std::vector<std::pair<std::string, Time>> sortedSchedule(schedule.begin(), schedule.end());
            std::sort(sortedSchedule.begin(), sortedSchedule.end(),
                      [](const auto& a, const auto& b) { return a.second < b.second; });
            for (const auto& [stop, time] : sortedSchedule) {
                std::cout << "  " << stop << " - " << time << '\n';
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void showAllTrips(const TransportSystem& system) {
    const auto& trips = system.getTrips();
    if (trips.empty()) {
        std::cout << "\nВ системе нет рейсов.\n";
        return;
    }
    std::cout << "\nВсе рейсы в системе:\n";
    for (const auto& trip : trips) {
        std::cout << "Рейс " << trip->getTripId()
                  << ": Маршрут " << trip->getRoute()->getNumber()
                  << ", отправление: " << trip->getStartTime()
                  << ", водитель: " << trip->getDriver()->getFullName()
                  << ", транспорт: " << trip->getVehicle()->getLicensePlate() << '\n';
    }
}

void runGuestMode(TransportSystem& system) {
    int choice;
    bool running = true;

    while (running) {
        displayGuestMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный формат ввода. Введите число.\n";
            continue;
        }
        std::cin.ignore();

        try {
            switch (choice) {
                case 0: running = false; break;
                case 1: viewTransportScheduleGuest(system); break;
                case 2: viewStopTimetableGuest(system); break;
                case 3: searchRoutes(system); break;
                default: std::cout << "Неверный выбор.\n";
            }
        } catch (const InputException& e) {
            std::cout << e.what() << '\n';
        } catch (const FileException& e) {
            std::cout << "Ошибка файла: " << e.what() << '\n';
        } catch (const ContainerException& e) {
            std::cout << "Ошибка данных: " << e.what() << '\n';
        } catch (const TransportException& e) {
            std::cout << "Ошибка системы: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Неожиданная ошибка: " << e.what() << '\n';
        }
    }
}

void runAdminMode(TransportSystem& system) {
    std::string username, password;

    std::cout << "Введите логин: ";
    std::getline(std::cin, username);
    std::cout << "Введите пароль: ";
    std::getline(std::cin, password);

    if (!system.authenticateAdmin(username, password)) {
        std::cout << "Неверный логин или пароль!\n";
        return;
    }

    std::cout << "Успешный вход!\n";

    int choice;
    bool running = true;

    while (running) {
        displayAdminMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный формат ввода. Введите число.\n";
            continue;
        }
        std::cin.ignore();

        try {
            switch (choice) {
                case 1: viewTransportSchedule(system); break;
                case 2: viewStopTimetableAdmin(system); break;
                case 3: searchRoutes(system); break;
                case 4: calculateArrivalTime(system); break;
                case 5: adminAddRoute(system); break;
                case 6: adminAddTrip(system); break;
                case 7: adminAddVehicle(system); break;
                case 8: adminAddStop(system); break;
                case 9: adminAddDriver(system); break;
                case 10: {
                    const auto& routes = system.getRoutes();
                    if (routes.empty()) {
                        std::cout << "\nВ системе нет маршрутов для удаления.\n";
                        break;
                    }
                    
                    std::cout << "\n=== ДОСТУПНЫЕ МАРШРУТЫ ДЛЯ УДАЛЕНИЯ ===\n";
                    for (size_t i = 0; i < routes.size(); ++i) {
                        std::cout << (i + 1) << ". Маршрут " << routes[i]->getNumber()
                                  << " (" << routes[i]->getVehicleType() << "): "
                                  << routes[i]->getStartStop() << " → " << routes[i]->getEndStop() << "\n";
                    }
                    std::cout << "==========================================\n";
                    std::cout << "Выберите номер маршрута для удаления: ";
                    
                    int routeChoice;
                    if (!(std::cin >> routeChoice)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        throw InputException("Неверный формат ввода");
                    }
                    std::cin.ignore();
                    
                    if (routeChoice < 1 || routeChoice > static_cast<int>(routes.size())) {
                        throw InputException("Неверный выбор маршрута. Допустимые значения: 1-" + std::to_string(routes.size()));
                    }
                    
                    int routeNumber = routes[routeChoice - 1]->getNumber();
                    system.removeRoute(routeNumber);
                    break;
                }
                case 11: {
                    const auto& trips = system.getTrips();
                    if (trips.empty()) {
                        std::cout << "\nВ системе нет рейсов для удаления.\n";
                        break;
                    }
                    
                    std::cout << "\n=== ДОСТУПНЫЕ РЕЙСЫ ДЛЯ УДАЛЕНИЯ ===\n";
                    for (size_t i = 0; i < trips.size(); ++i) {
                        std::cout << (i + 1) << ". Рейс " << trips[i]->getTripId()
                                  << ": Маршрут " << trips[i]->getRoute()->getNumber()
                                  << ", отправление: " << trips[i]->getStartTime()
                                  << ", водитель: " << trips[i]->getDriver()->getFullName()
                                  << ", транспорт: " << trips[i]->getVehicle()->getLicensePlate() << "\n";
                    }
                    std::cout << "=====================================\n";
                    std::cout << "Выберите номер рейса для удаления: ";
                    
                    int tripChoice;
                    if (!(std::cin >> tripChoice)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        throw InputException("Неверный формат ввода");
                    }
                    std::cin.ignore();
                    
                    if (tripChoice < 1 || tripChoice > static_cast<int>(trips.size())) {
                        throw InputException("Неверный выбор рейса. Допустимые значения: 1-" + std::to_string(trips.size()));
                    }
                    
                    int tripId = trips[tripChoice - 1]->getTripId();
                    system.removeTrip(tripId);
                    break;
                }
                case 12: {
                    system.displayAllRoutes();
                    system.displayAllTrips();
                    system.displayAllVehicles();
                    system.displayAllStops();
                    system.displayAllDrivers();
                    break;
                }
                case 13: system.saveData(); break;
                case 14: {
                    if (system.canUndo()) {
                        std::string lastAction = system.getLastCommandDescription();
                        system.undo();
                        std::cout << "Отменено действие: " << lastAction << "\n";
                    } else {
                        std::cout << "Нет действий для отмены.\n";
                    }
                    break;
                }
                case 15: 
                    system.saveData();
                    std::cout << "Данные сохранены. Выход из административного режима.\n";
                    running = false; 
                    break;
                default: std::cout << "Неверный выбор.\n";
            }
        } catch (const InputException& e) {
            std::cout << e.what() << '\n';
        } catch (const FileException& e) {
            std::cout << "Ошибка файла: " << e.what() << '\n';
        } catch (const ContainerException& e) {
            std::cout << "Ошибка данных: " << e.what() << '\n';
        } catch (const TransportException& e) {
            std::cout << "Ошибка системы: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Неожиданная ошибка: " << e.what() << '\n';
        }
    }
}

