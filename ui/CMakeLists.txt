cmake_minimum_required(VERSION 3.16.3)

project(mosaik-ui)

add_subdirectory(midi-ui)
add_subdirectory(touch-ui)

add_library(${PROJECT_NAME} STATIC
        Types.h
        ParameterDescriptor.h
        ToolboxDefinition.h
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} mosaik-touchui mosaik-midiui ${LIBS_LIBRARIES})


