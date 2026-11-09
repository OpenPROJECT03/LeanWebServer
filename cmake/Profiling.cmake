# cmake/Profiling.cmake

function(lean_set_target_profiling TARGET_NAME)
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

    if(LEAN_WEB_SERVER_ENABLE_PROFILING)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
            target_compile_options(
                ${TARGET_NAME}
                ${ARG_MODE}
                -fno-omit-frame-pointer
                -fno-optimize-sibling-calls
                -g
            )
        else()
            message(
                WARNING
                "Profiling frame pointer flags are not supported for compiler ${CMAKE_CXX_COMPILER_ID}"
            )
        endif()
    endif()
endfunction()

add_library(lean_profiling INTERFACE)
add_library(LeanWebServer::profiling ALIAS lean_profiling)
lean_set_target_profiling(lean_profiling MODE INTERFACE)

if(
    LEAN_WEB_SERVER_ENABLE_PROFILING
    AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU"
)
    message(
        STATUS
        "Profiling flags enabled: -fno-omit-frame-pointer, -fno-optimize-sibling-calls, -g"
    )
endif()
