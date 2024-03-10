cmake_minimum_required(VERSION 3.21)
project(client)
message(STATUS "start running cmake...")
set(CMAKE_CXX_STANDARD 20)
# setting CryptoPP path based on guest arch.
EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Architecture: ${ARCHITECTURE}")
if (${ARCHITECTURE} STREQUAL "64Bit")
    set(cryptopp_DIR x64-osx/share/cryptopp)
else ()
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(cryptopp_DIR arm64-osx/share/cryptopp)
endif ()

include_directories(.)
add_executable(client
        constants.h
        client.cpp
        client.h
        crypto_handler.cpp
        crypto_handler.h
        Main.cpp
        exceptions.h
        Request.cpp
        Request.h
        Response.cpp
        Response.h
        UsersList.cpp
        UsersList.h
        Utils.cpp
        Utils.h)

find_package(Boost 1.78.0 REQUIRED COMPONENTS system filesystem random unit_test_framework)
find_package(cryptopp CONFIG REQUIRED)

if (Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")

    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif ()

target_link_libraries(client PRIVATE cryptopp-static ${Boost_LIBRARIES})




