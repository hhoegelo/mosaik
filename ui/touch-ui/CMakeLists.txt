cmake_minimum_required(VERSION 3.16.3)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-3.0)

add_library(mosaik-touchui STATIC
        tiles/Tiles.cpp
        tiles/Tiles.h
        tiles/Tile.cpp
        toolbox/Toolboxes.h
        toolbox/Toolboxes.cpp
        controls/Checkbox.cpp
        controls/FloatScaleButton.cpp
        controls/StepButton.cpp
        controls/Label.cpp
        Ui.cpp
        Window.cpp
        Window.h)

target_compile_options(mosaik-touchui PRIVATE -fmodules-ts)
target_include_directories(mosaik-touchui PRIVATE ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-touchui mosaik-core ${LIBS_LIBRARIES})

