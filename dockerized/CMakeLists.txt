cmake_minimum_required(VERSION 3.20)

project(dockerized)

set(DOCKERNAME "mosaik-build-environment")
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/*)

add_custom_command(
        COMMENT "Create dockerized build environment"
        OUTPUT .pod
        DEPENDS Dockerfile
        COMMAND docker build -t ${DOCKERNAME} ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND touch .pod
)

add_custom_command(
        COMMENT "Build mosaik in dockerized environment"
        DEPENDS .pod
        DEPENDS ${SOURCE_FILES}
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin/mosaik
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin
        COMMAND docker run --rm -v ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin:/binary -v ${CMAKE_SOURCE_DIR}:/source -w /binary ${DOCKERNAME} cmake /source
        COMMAND docker run --rm -v ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin:/binary -v ${CMAKE_SOURCE_DIR}:/source -w /binary ${DOCKERNAME} make mosaik
)

add_custom_command(
        COMMENT "Run mosaik in dockerized environment"
        DEPENDS .pod
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin/mosaik
        OUTPUT .run

        COMMAND xhost +si:localuser:$ENV{USER}
        COMMAND xhost +local:docker

        COMMAND docker run -ti --rm --net=host
        -e DISPLAY=$ENV{DISPLAY}
        -e XDG_SESSION_ID=$ENV{XDG_SESSION_ID}
        -v "$ENV{HOME}/.Xauthority:/root/.Xauthority:rw"
        -v /tmp/.X11-unix:/tmp/.X11-unix
        -v $ENV{HOME}/:/root
        -v ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin:/binary
        -v ${CMAKE_SOURCE_DIR}:/source
        -w /binary
        --security-opt label=type:container_runtime_t
        --device /dev/snd ${DOCKERNAME} /binary/mosaik --alsa-out=\${AUDIO_OUT}
)

add_custom_target(dockerized-build-mosaik DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin/mosaik)
add_custom_target(dockerized-run-mosaik DEPENDS .run)
