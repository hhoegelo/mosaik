cmake_minimum_required(VERSION 3.16.3)

find_package(PkgConfig REQUIRED)
set(CMAKE_FIND_STATIC FIRST)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-3.0)

CPMAddPackage("gh:BramvdKroef/clessc#4176967fdfcb9ed571fbad659f8bec77e06a9d12")

add_library(mosaik-touchui STATIC
        tiles/Tiles.cpp
        tiles/Tiles.h
        tiles/Tile.cpp
        toolboxes/Toolboxes.h
        toolboxes/Toolboxes.cpp
        Application.cpp
        Window.cpp
        Window.h
        toolboxes/TileTools.cpp
        toolboxes/TileTools.h
        toolboxes/Waveform.cpp
        toolboxes/Waveform.h
        Interface.h
        Types.h
        toolboxes/SoftButtonGrid.cpp
        toolboxes/SoftButtonGrid.h
        toolboxes/KnobGrid.cpp
        toolboxes/KnobGrid.h
        LessToCss.cpp
        LessToCss.h
        tiles/WaveformThumb.cpp
        LevelMeter.cpp
        LevelMeter.h
        toolboxes/Toolbox.cpp
        toolboxes/Toolbox.h
        tools/WidgetTools.cpp
)

target_compile_options(mosaik-touchui PRIVATE)
target_include_directories(mosaik-touchui PUBLIC ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS} ${clessc_INCLUDE_DIRS})
target_link_libraries(mosaik-touchui mosaik-core mosaik-ui ${LIBS_LIBRARIES} less)

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
