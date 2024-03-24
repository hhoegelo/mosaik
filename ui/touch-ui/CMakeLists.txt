cmake_minimum_required(VERSION 3.16.3)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-3.0)

add_library(mosaik-touchui STATIC
        tiles/Tiles.cpp
        tiles/Tiles.h
        tiles/Tile.cpp
        toolbox/Toolboxes.h
        toolbox/Toolboxes.cpp
        Ui.cpp
        Window.cpp
        Window.h
        toolbox/GlobalTools.cpp
        toolbox/GlobalTools.h
        toolbox/TileTools.cpp
        toolbox/TileTools.h
        toolbox/Waveform.cpp
        toolbox/Waveform.h
        Interface.h
        Types.h
)

target_compile_options(mosaik-touchui PRIVATE)
target_include_directories(mosaik-touchui PUBLIC ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(mosaik-touchui mosaik-core ${LIBS_LIBRARIES})

