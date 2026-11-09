# cmake/Sanitizers.cmake

function(lean_set_target_sanitizers TARGET_NAME)
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

    if(LEAN_WEB_SERVER_ENABLE_SANITIZERS)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
            target_compile_options(
                ${TARGET_NAME}
                ${ARG_MODE}
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
                -fno-optimize-sibling-calls
            )
            target_link_options(
                ${TARGET_NAME}
                ${ARG_MODE}
                -fsanitize=address,undefined
            )
        else()
            message(
                WARNING
                "Sanitizers are not supported for compiler ${CMAKE_CXX_COMPILER_ID}"
            )
        endif()
    endif()
endfunction()

add_library(lean_sanitizers INTERFACE)
add_library(LeanWebServer::sanitizers ALIAS lean_sanitizers)
lean_set_target_sanitizers(lean_sanitizers MODE INTERFACE)

if(
    LEAN_WEB_SERVER_ENABLE_SANITIZERS
    AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU"
)
    message(
        STATUS
        "Sanitizers enabled: AddressSanitizer, UndefinedBehaviorSanitizer"
    )
endif()
