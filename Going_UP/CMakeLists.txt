# CMakeList.txt: проект CMake для Going_UP; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Going_UP "Going_UP.cpp" "Player.cpp" "Player.h" "Structures.h" "Going_UP.h" "Platforms.h" "Platforms.cpp" "Config.h" "Config.cpp" "Screen.cpp" "Artifact.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Going_UP PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Going_UP PRIVATE SDL3::SDL3 nlohmann_json::nlohmann_json SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image)

add_custom_command(
    TARGET Going_UP POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:Going_UP>
    VERBATIM)

 add_custom_command(
     TARGET Going_UP POST_BUILD
     COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:Going_UP>
     VERBATIM)

 add_custom_command(
     TARGET Going_UP POST_BUILD
     COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_image::SDL3_image-shared> $<TARGET_FILE_DIR:Going_UP>
     VERBATIM)