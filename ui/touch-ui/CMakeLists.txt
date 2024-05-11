cmake_minimum_required(VERSION 3.16.3)

find_package(PkgConfig REQUIRED)
set(CMAKE_FIND_STATIC FIRST)
pkg_check_modules(LIBS REQUIRED IMPORTED_TARGET gtkmm-3.0)

CPMAddPackage("gh:BramvdKroef/clessc#4176967fdfcb9ed571fbad659f8bec77e06a9d12")

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
        sections/SectionWrapper.h
        sections/main/Main.cpp
        sections/mixer/Mixer.cpp
        LessToCss.cpp
        LessToCss.h
        sections/tiles/WaveformThumb.cpp
        sections/LevelMeter.cpp
        sections/LevelMeter.h
        sections/toolboxes/Toolbox.cpp
        sections/toolboxes/Toolbox.h
        tools/WidgetTools.cpp
        sections/mixer/Channel.cpp
        sections/mixer/Channel.h
)

target_compile_options(mosaik-touchui PRIVATE)
target_include_directories(mosaik-touchui PUBLIC ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS} ${clessc_INCLUDE_DIRS})
target_link_libraries(mosaik-touchui mosaik-core mosaik-ui ${LIBS_LIBRARIES} less)

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
