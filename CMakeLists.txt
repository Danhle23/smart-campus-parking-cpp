cmake_minimum_required(VERSION 3.16)
project(SmartCampusParking LANGUAGES CXX)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PARKING_WARNINGS -Wall -Wextra -Wpedantic -Wconversion -Wshadow)

add_library(parking_core
    src/DataLoader.cpp
    src/FeeCalculator.cpp
    src/FuzzySearch.cpp
    src/ParkingGraph.cpp
    src/ParkingSystem.cpp
    src/Types.cpp
)
target_include_directories(parking_core PUBLIC include)
target_compile_options(parking_core PRIVATE ${PARKING_WARNINGS})

if(ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(parking_core PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(parking_core PRIVATE -fsanitize=address,undefined)
endif()

add_executable(smart_parking app/main.cpp)
target_link_libraries(smart_parking PRIVATE parking_core)
target_compile_options(smart_parking PRIVATE ${PARKING_WARNINGS})

add_executable(smart_parking_tests tests/test_main.cpp)
target_link_libraries(smart_parking_tests PRIVATE parking_core)
target_compile_options(smart_parking_tests PRIVATE ${PARKING_WARNINGS})

if(ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    foreach(target smart_parking smart_parking_tests)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endforeach()
endif()

enable_testing()
add_test(NAME smart_parking_all_tests COMMAND smart_parking_tests)
