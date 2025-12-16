cmake_minimum_required(VERSION 3.16)
project(vika's_kursach VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исходные файлы проекта
set(SOURCES
        main.cpp
        ui.cpp
        exceptions.cpp
        stop.cpp
        time.cpp
        vehicle.cpp
        electric_transport.cpp
        fuel_transport.cpp
        tram.cpp
        trolleybus.cpp
        bus.cpp
        driver.cpp
        route.cpp
        trip.cpp
        journey.cpp
        algorithm.cpp
        journey_planner.cpp
        driver_schedule.cpp
        data_manager.cpp
        command.cpp
        commands.cpp
        transport_system.cpp
)

# Создание исполняемого файла
add_executable(vikas_kursach ${SOURCES})

# Для Windows: дополнительные настройки
if(WIN32)
    target_compile_definitions(vikas_kursach PRIVATE _WIN32_WINNT=0x0601)
    # Установка кодировки UTF-8 для Windows
    target_compile_definitions(vikas_kursach PRIVATE UNICODE _UNICODE)
endif()

