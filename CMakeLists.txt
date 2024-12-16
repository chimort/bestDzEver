cmake_minimum_required(VERSION 3.10)
project(bestDz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(DEFINED TYPES)
    execute_process(
        COMMAND python3 generator.py "${TYPES}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    add_executable(bestDz main.cpp)

    target_include_directories(bestDz PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(FATAL_ERROR "TYPES variable is not defined. Use -DTYPES=<value>.")
endif()
