cmake_minimum_required(VERSION 3.21)

project(client)

message(STATUS "start running cmake...")

set(CMAKE_CXX_STANDARD 20)

# Detect the operating system and package manager
if (APPLE)
    set(MACOS TRUE)
elseif (UNIX AND NOT APPLE)
    set(LINUX TRUE)
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

# Find Boost package
find_package(Boost 1.74.0 REQUIRED COMPONENTS system filesystem random unit_test_framework)
if (NOT Boost_FOUND)
    message(STATUS "Boost not found. Installing Boost using ${PACKAGE_MANAGER}...")
    if (MACOS)
        execute_process(COMMAND brew install boost)
    elseif (LINUX)
        execute_process(COMMAND sudo apt update)
        execute_process(COMMAND sudo apt install -y libboost-all-dev)
    endif ()
    find_package(Boost 1.74.0 REQUIRED COMPONENTS system filesystem random unit_test_framework)
endif ()

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
include_directories(${Boost_INCLUDE_DIRS})

# Find Cryptopp package (only on macOS)
if (MACOS)
    set(CRYPTOPP_INCLUDE_DIR "/opt/homebrew/Cellar/cryptopp/8.9.0/include")
    set(CRYPTOPP_LIBRARIES "/opt/homebrew/Cellar/cryptopp/8.9.0/lib/libcryptopp.a")

    if (NOT EXISTS ${CRYPTOPP_INCLUDE_DIR} OR NOT EXISTS ${CRYPTOPP_LIBRARIES})
        message(STATUS "Cryptopp not found. Installing Cryptopp using brew...")
        execute_process(COMMAND brew install cryptopp)
    endif ()

else ()
    set(CRYPTOPP_INCLUDE_DIRS "/usr/include/cryptopp")
    set(CRYPTOPP_LIBRARIES "/usr/lib/aarch64-linux-gnu/libcryptopp.a")
    if (NOT CRYPTOPP_INCLUDE_DIRS OR NOT CRYPTOPP_LIBRARIES)
        message(STATUS "Cryptopp not found. Installing Cryptopp using ${PACKAGE_MANAGER}...")
        execute_process(COMMAND sudo apt update)
        execute_process(COMMAND sudo apt install -y cmake make gcc build-essential libcrypto++-dev)
    endif ()
endif ()

include_directories(${CRYPTOPP_INCLUDE_DIRS})
message(STATUS "CRYPTOPP_INCLUDE_DIRS: ${CRYPTOPP_INCLUDE_DIRS}")
message(STATUS "CRYPTOPP_LIBRARIES: ${CRYPTOPP_LIBRARIES}")

add_executable(client
        constants.h
        client.cpp
        client.h
        crypto_handler.cpp
        crypto_handler.h
        main.cpp
        exceptions.h
        requests.cpp
        requests.h
        responses.cpp
        responses.h
        users.cpp
        users.h
        utils.cpp
        utils.h
)

target_link_libraries(client PRIVATE ${CRYPTOPP_LIBRARIES} ${Boost_LIBRARIES})