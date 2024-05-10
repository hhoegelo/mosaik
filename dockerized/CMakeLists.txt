cmake_minimum_required(VERSION 3.16.3)

project(dockerized)

set(DOCKERNAME_ARM "mosaik-cross-build-environment")
set(DOCKERNAME_X86 "mosaik-build-environment")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/*)

configure_file(Dockerfile-arm arm/Dockerfile)
configure_file(Dockerfile-x86 x86/Dockerfile)

add_custom_command(
        COMMENT "Create dockerized cross build environment"
        OUTPUT .pod-arm
        DEPENDS Dockerfile-arm
        COMMAND docker build -t ${DOCKERNAME_ARM} ${CMAKE_CURRENT_BINARY_DIR}/arm
        COMMAND touch .pod
)

add_custom_command(
        COMMENT "Create dockerized build environment"
        OUTPUT .pod-x86
        DEPENDS Dockerfile-x86
        COMMAND docker build -t ${DOCKERNAME_X86} ${CMAKE_CURRENT_BINARY_DIR}/x86
        COMMAND touch .pod
)

add_custom_command(
        COMMENT "Build mosaik in dockerized arm environment"
        DEPENDS .pod-arm
        DEPENDS ${SOURCE_FILES}
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-arm/mosaik
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-arm
        COMMAND docker run --platform linux/arm64 --rm -v ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-arm:/binary -v ${CMAKE_SOURCE_DIR}:/source -w /binary ${DOCKERNAME_ARM} cmake /source
        COMMAND docker run --platform linux/arm64 --rm -v ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-arm:/binary -v ${CMAKE_SOURCE_DIR}:/source -w /binary ${DOCKERNAME_ARM} make mosaik
)

add_custom_command(
        COMMENT "Build mosaik in dockerized x86 environment"
        DEPENDS .pod-x86
        DEPENDS ${SOURCE_FILES}
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-x86/mosaik
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-x86
        COMMAND docker run --rm -v ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-x86:/binary -v ${CMAKE_SOURCE_DIR}:/source -w /binary ${DOCKERNAME_X86} cmake /source
        COMMAND docker run --rm -v ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-x86:/binary -v ${CMAKE_SOURCE_DIR}:/source -w /binary ${DOCKERNAME_X86} make mosaik
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

add_custom_target(dockerized-build-mosaik-arm DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-arm/mosaik)
add_custom_target(dockerized-build-mosaik-x86 DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dockerized-bin-x86/mosaik)
add_custom_target(dockerized-run-mosaik DEPENDS .run)
