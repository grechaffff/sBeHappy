set(PROJECT_SOURCES
    tcp_server_module/tcp_server.cpp
)

set(PROJECT_LIBRARY_NAME ${CMAKE_PROJECT_NAME}_library)

add_library(${PROJECT_LIBRARY_NAME} STATIC ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_LIBRARY_NAME}
    INTERFACE
    pqxx pq
    Boost::system
    OpenSSL::SSL
    bcrypt
    spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
    fmt::fmt
)
