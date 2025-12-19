cmake_minimum_required(VERSION 3.15)
project(laba7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основное приложение
add_executable(laba7
    main.cpp
    npc.cpp
    npc_types.cpp
    game_manager.cpp
    npc_factory.cpp
)

# Заголовочные файлы
target_include_directories(laba7 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Флаги компиляции
if(MSVC)
    target_compile_options(laba7 PRIVATE /W4 /WX)
else()
    target_compile_options(laba7 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Тесты
enable_testing()

add_executable(npc_tests
    tests/test_npc.cpp
    tests/test_game_manager.cpp
    npc.cpp
    npc_types.cpp
    game_manager.cpp
    npc_factory.cpp
)

target_include_directories(npc_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME NPC_Tests COMMAND npc_tests)

# Опционально: отдельный пример
add_executable(npc_example
    examples/simple_example.cpp
    npc.cpp
    npc_types.cpp
    game_manager.cpp
    npc_factory.cpp
)

target_include_directories(npc_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
