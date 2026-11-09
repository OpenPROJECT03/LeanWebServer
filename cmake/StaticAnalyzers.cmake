# cmake/StaticAnalyzers.cmake

if(LEAN_WEB_SERVER_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY
            "${CLANG_TIDY_EXE};--extra-arg-before=-Wno-unknown-warning-option"
        )
    else()
        message(
            WARNING
            "LEAN_WEB_SERVER_ENABLE_CLANG_TIDY is ON, but clang-tidy executable was not found"
        )
    endif()
endif()
