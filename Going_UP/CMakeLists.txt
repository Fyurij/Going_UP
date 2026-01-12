# CMakeList.txt: проект CMake для Going_UP; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Going_UP "Going_UP.cpp" "Player.cpp" "Player.h" "Structures.h" "Going_UP.h" "Platforms.h" "Platforms.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Going_UP PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Going_UP PRIVATE SDL3::SDL3)

add_custom_command(
    TARGET Going_UP POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:Going_UP>
    VERBATIM)