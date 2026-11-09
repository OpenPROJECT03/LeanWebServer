# app/CMakeLists.txt

add_executable(lean_web_server main.cpp)

target_link_libraries(
    lean_web_server
    PRIVATE
        LeanWebServer::core
        LeanWebServer::warnings
        LeanWebServer::sanitizers
        LeanWebServer::profiling
)

target_compile_features(lean_web_server PRIVATE cxx_std_17)

if(LEAN_WEB_SERVER_INSTALL)
    install(TARGETS lean_web_server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
