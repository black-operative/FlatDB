cmake_minimum_required(VERSION 3.15)
project(FlatDB)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/Main.cpp
    src/JSONDB.cpp
    src/Parser.cpp
    src/Utils.cpp
)

# Add executable
add_executable(FlatDB ${SOURCES})

# Add nlohmann_json from system or vcpkg or use FetchContent
# Option 1: If installed globally or via vcpkg
# find_package(nlohmann_json CONFIG REQUIRED)
# target_link_libraries(FlatDB PRIVATE nlohmann_json::nlohmann_json)

# Option 2 (uncomment if you prefer FetchContent to auto-download):
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)
target_link_libraries(FlatDB PRIVATE nlohmann_json::nlohmann_json)
