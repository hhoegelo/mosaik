cmake_minimum_required(VERSION 3.16.3)

project(mosaik-ui)

add_subdirectory(midi-ui)
add_subdirectory(touch-ui)

add_library(${PROJECT_NAME} STATIC
        Controller.cpp
        Controller.h
        Types.h
        ParameterDescriptor.h
        ToolboxDefinition.h
        StepWizard.h
        StepWizard.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR} ${LIBS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBS_LIBRARIES})


