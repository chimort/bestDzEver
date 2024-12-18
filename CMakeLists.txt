cmake_minimum_required(VERSION 3.10)
project(bestDz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(print_info MESSAGE_TEXT)
    message(STATUS "${MESSAGE_TEXT}")
endfunction()

print_info("\nСОВЕТЫ ПО ИСПОЛЬЗОВАНИЮ")
print_info("1) МНОГО ТИПОВ ПРИ ГЕНЕРАЦИИ ЛОМАЮТ ПРОГРАММУ ПОТОМУ ЧТО У ВАС НЕ ХВАТИТ ПАМЯТИ НА ЭТО,\n ПОЭТОМУ ЖЕЛАТЕЛЬНО ИСПОЛЬЗОВАТЬ НЕ БОЛЬШЕ 3Х ТИПОВ)\n ИЛИ ПРОСТО РАСШИРИТЬ СТЕК.")
print_info("2) ТИПЫ С МАЛЕНЬКОЙ ТОЧНОСТЬЮ МОГУТ НЕ РАБОТАТЬ, НО ЭТО УЖЕ ПРОБЛЕМЫ РЕАЛИЗАЦИИ ФИЗИКИ.")
print_info("3) СЛИШКОМ БОЛЬШИЕ ТИПЫ ТОЖЕ МОГУТ СЛОМАТЬ ПРОГРАММУ, ОПЯТЬ ЖЕ ИЗ-ЗА ИЗНАЧАЛЬНОЙ РЕАЛИЗАЦИИ.\n")


if(DEFINED TYPES)
    execute_process(
        COMMAND python3 generator.py "${TYPES}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    add_executable(bestDz main.cpp)
    target_include_directories(bestDz PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    print_info("ГЕНЕРАЦИЯ ЗАВЕРШЕНА УСПЕШНО.")
else()
    print_info("ОШИБКА: TYPES НЕ ОПРЕДЕЛЕН! ИСПОЛЬЗУЙТЕ -DTYPES=<value>.")
    message(FATAL_ERROR "TYPES variable is not defined. Use -DTYPES=<value>.")
endif()
