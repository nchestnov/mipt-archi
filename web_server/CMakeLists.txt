cmake_minimum_required(VERSION 3.10)
project(Archi C CXX)

set(CMAKE_CXX_STANDARD 17)


find_package(Threads)
find_package(OpenSSL)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)
find_package(CppKafka REQUIRED)
find_package(Boost COMPONENTS filesystem system program_options regex REQUIRED)

if (NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found!")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

add_executable(webserver webserver.cpp models/person.cpp config/config.cpp db_connector/db_connector.cpp)
add_executable(worker worker.cpp models/person.cpp config/config.cpp db_connector/db_connector.cpp)

target_include_directories(webserver PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(worker PRIVATE "${CMAKE_BINARY_DIR}")

target_compile_options(webserver PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(worker PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_libraries(webserver PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        "rdkafka++"
        CppKafka::cppkafka
        ${Boost_LIBRARIES})

target_link_libraries(worker PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        "rdkafka++"
        CppKafka::cppkafka
        ${Boost_LIBRARIES})

set_target_properties(webserver PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(webserver PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

set_target_properties(worker PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(worker PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)