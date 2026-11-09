# cmake/CompilerWarnings.cmake

function(lean_set_target_warnings TARGET_NAME)
    set(options)
    set(oneValueArgs MODE)
    set(multiValueArgs)
    cmake_parse_arguments(
        PARSE_ARGV 1
        ARG
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
    )

    if(NOT ARG_MODE)
        set(ARG_MODE PRIVATE)
    endif()

    if(LEAN_WEB_SERVER_ENABLE_WARNINGS)
        set(PROJECT_WARNINGS
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        )

        if(LEAN_WEB_SERVER_WARNINGS_AS_ERRORS)
            list(APPEND PROJECT_WARNINGS -Werror)
        endif()

        target_compile_options(
            ${TARGET_NAME}
            ${ARG_MODE}
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:${PROJECT_WARNINGS}>
        )
    endif()
endfunction()

add_library(lean_warnings INTERFACE)
add_library(LeanWebServer::warnings ALIAS lean_warnings)
lean_set_target_warnings(lean_warnings MODE INTERFACE)
