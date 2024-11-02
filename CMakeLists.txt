cmake_minimum_required(VERSION 3.10)

project(VFrown3DS)

set(CMAKE_C_STANDARD 99)

set(VFCORE_DIR ${CMAKE_SOURCE_DIR}/src/core)
set(VFCORE_SRC
	${VFCORE_DIR}/bus.c
	${VFCORE_DIR}/cpu.c
	${VFCORE_DIR}/ppu.c
	${VFCORE_DIR}/spu.c
	${VFCORE_DIR}/timer.c
	${VFCORE_DIR}/vsmile.c
	${VFCORE_DIR}/hw/controller.c
	${VFCORE_DIR}/hw/dma.c
	${VFCORE_DIR}/hw/gpio.c
	${VFCORE_DIR}/hw/misc.c
	${VFCORE_DIR}/hw/timers.c
	${VFCORE_DIR}/hw/uart.c
)

add_library(vfcore STATIC ${VFCORE_SRC})

set(VFBACKEND_DIR ${CMAKE_SOURCE_DIR}/src/backend)
set(VFBACKEND_SRC
	${VFBACKEND_DIR}/backend.c
	${VFBACKEND_DIR}/input.c
	${VFBACKEND_DIR}/libs.c
	${VFBACKEND_DIR}/userSettings.c
)
add_library(vfbackend STATIC ${VFBACKEND_SRC})

add_executable(vfrown3ds
	${CMAKE_SOURCE_DIR}/src/main.c
	${CMAKE_SOURCE_DIR}/src/common.c
)
target_link_libraries(vfrown3ds vfcore vfbackend)
