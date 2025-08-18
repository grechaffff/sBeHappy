set(PROJECT_SOURCES application.cpp tcp_server.cpp)

set(PROJECT_LIBRARY_NAME ${CMAKE_PROJECT_NAME}_library)

add_library(${PROJECT_LIBRARY_NAME} STATIC ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_LIBRARY_NAME}
    PRIVATE
    pqxx pq
    Boost::system
    OpenSSL::SSL
    bcrypt
    spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
    fmt::fmt
)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_LIBRARY_NAME})