cmake_minimum_required(VERSION 3.10)
project(Archi C CXX)

set(CMAKE_CXX_STANDARD 17)

#enable_testing()

#find_package(GTest REQUIRED)
find_package(Threads)
find_package(OpenSSL)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)
find_package(Java 1.8 REQUIRED)
find_package(JNI REQUIRED)

if (NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found!")
endif()

include_directories(SYSTEM ${IGNITE_INCLUDE_DIR} ${JNI_INCLUDE_DIRS})
include_directories(${Poco_INCLUDE_DIRS})
#include_directories(${GTEST_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

add_executable(main main.cpp models/person.cpp config/config.cpp db_connector/db_connector.cpp db_connector/cache.cpp db_connector/cache.h)
#add_executable(tests tests.cpp models/person.cpp config/config.cpp db_connector/db_connector.cpp)

target_compile_options(main PRIVATE -Wall -Wextra -pedantic -Werror)
#target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_libraries(main PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        "ignite-binary"
        "ignite"
        "ignite-common"
        ${IGNITE_JNI_LIB}
        "ignite-thin-client")
#target_link_libraries(tests PRIVATE
#        ${GTEST_LIBRARIES}
#        ${CMAKE_THREAD_LIBS_INIT}
#        ${Poco_LIBRARIES}
#        "PocoData"
#        "PocoDataMySQL"
#        "mysqlclient"
#        )

set_target_properties(
        main
        #tests
        PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

#enable_testing()
#add_test(tests gtests)
#include(GoogleTest)
#gtest_discover_tests(tests)