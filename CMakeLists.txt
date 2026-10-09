cmake_minimum_required(VERSION 3.20)
project(simpletron CXX)

# Defines the C++ version 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# puts simpletronin a library so it can be used by other targets
add_library(simpletron_core
    src/simpletron.cpp
)
target_include_directories(simpletron_core PUBLIC include)

# makes the simpletron executable and links it to the simpletron_core library
add_executable(simpletron
    src/main.cpp
)
target_link_libraries(simpletron PRIVATE simpletron_core)


# 3. GTest configuration
include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(simpletron_tests
        tests/test_simpletron.cpp
    )
    target_link_libraries(simpletron_tests PRIVATE
        simpletron_core
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(simpletron_tests)
endif()