cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.66.0 REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

file(GLOB SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE pqxx pq)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
    bcrypt
)