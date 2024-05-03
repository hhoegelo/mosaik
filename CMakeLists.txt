cmake_minimum_required(VERSION 3.16.3)

project(mosaik)

include(cmake/CPM.cmake)

add_subdirectory(dockerized)

option(ENABLE_TESTING OFF)

if (ENABLE_TESTING)
    Include(FetchContent)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.4.0
    )

    FetchContent_MakeAvailable(Catch2)
    include(CTest)
    include(Catch)

    add_executable(mosaik-tests tests/MosaikTests.cpp)

    target_compile_definitions(mosaik-tests PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")
    target_include_directories(mosaik-tests PRIVATE ./)
    target_link_libraries(mosaik-tests PRIVATE Catch2::Catch2WithMain)
    catch_discover_tests(mosaik-tests)
endif ()

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(SAMPLERATE=48000)
add_compile_definitions(NUM_STEPS=64)

add_compile_definitions(NUM_TILE_COLUMNS=6)
add_compile_definitions(NUM_TILE_ROWS=6)
add_compile_definitions(NUM_TILES=36)

find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED program_options)

pkg_check_modules(LIBS IMPORTED_TARGET alsa glibmm-2.4 giomm-2.4 gtkmm-3.0 gstreamermm-1.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (LIBS_FOUND)
    set(RESOURCES_DIR ${CMAKE_INSTALL_FULL_DATADIR}/mosaik/)
    set(SOURCES_DIR ${CMAKE_SOURCE_DIR})

    add_compile_definitions(RESOURCES_DIR="${RESOURCES_DIR}")
    add_compile_definitions(SOURCES_DIR="${SOURCES_DIR}")
    add_compile_definitions(DEBUG_BUILD=$<IF:$<CONFIG:Debug>,true,false>)
    add_compile_definitions(RELEASE_BUILD=$<IF:$<CONFIG:Debug>,false,true>)

    add_subdirectory(core)
    add_subdirectory(dsp)
    add_subdirectory(audio)
    add_subdirectory(midi)
    add_subdirectory(ui)
    add_subdirectory(tools)

    add_executable(mosaik main.cpp)

    target_include_directories(mosaik PRIVATE ${LIBS_INCLUDE_DIRS})
    target_link_libraries(mosaik PRIVATE ${LIBS_LIBRARIES}
            mosaik-core
            mosaik-dsp
            mosaik-audio
            mosaik-touchui
            mosaik-midiui
            Boost::program_options)

    install(FILES ui/touch-ui/styles.less DESTINATION ${RESOURCES_DIR} PERMISSIONS OWNER_WRITE OWNER_READ)
else ()
    message("Not all required libs were found, but you can still use the dockerized build env, like this:")
    message(" make dockerized-build-mosaik")
    message(" make dockerized-run-mosaik AUDIO_OUT=hw:3,0")
endif ()
