cmake_minimum_required(VERSION 3.16.3)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-3.0)

CPMAddPackage(
        NAME "clessc"
        GITHUB_REPOSITORY "BramvdKroef/clessc"
        GIT_TAG "4176967fdfcb9ed571fbad659f8bec77e06a9d12"
)

add_library(mosaik-touchui STATIC
        sections/tiles/Tiles.cpp
        sections/tiles/Tiles.h
        sections/tiles/Tile.cpp
        sections/toolboxes/Toolboxes.h
        sections/toolboxes/Toolboxes.cpp
        Application.cpp
        Window.cpp
        Window.h
        sections/toolboxes/TileTools.cpp
        sections/toolboxes/TileTools.h
        sections/toolboxes/Waveform.cpp
        sections/toolboxes/Waveform.h
        Interface.h
        Types.h
        sections/toolboxes/SoftButtonGrid.cpp
        sections/toolboxes/SoftButtonGrid.h
        sections/toolboxes/KnobGrid.cpp
        sections/toolboxes/KnobGrid.h
        sections/toolboxes/Playground.cpp
        sections/toolboxes/Playground.h
        sections/SectionWrapper.h
        sections/main/Main.cpp
        sections/mixer/Mixer.cpp
        LessToCss.cpp
        LessToCss.h
        sections/tiles/WaveformThumb.cpp
        sections/tiles/LevelMeter.cpp
        sections/tiles/LevelMeter.h
        sections/toolboxes/Toolbox.cpp
        sections/toolboxes/Toolbox.h
        Display.h
        Display.cpp
        tools/WidgetTools.cpp
)

target_compile_options(mosaik-touchui PRIVATE)
target_include_directories(mosaik-touchui PUBLIC ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS} ${clessc_INCLUDE_DIRS})
target_link_libraries(mosaik-touchui mosaik-core ${LIBS_LIBRARIES} less)

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
