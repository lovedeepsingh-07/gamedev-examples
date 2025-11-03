cmake_minimum_required(VERSION 3.15)

project(${EXAMPLE_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL_DIR deps)
add_subdirectory(${EXTERNAL_DIR}/raylib)
add_subdirectory(${EXTERNAL_DIR}/flecs)

if (EXISTS "examples/${EXAMPLE_NAME}.cpp")
	set(SRC_FILES examples/${EXAMPLE_NAME}.cpp)
else()
	if (EXISTS "examples/${EXAMPLE_NAME}/main.cpp")
		file(GLOB_RECURSE SRC_FILES examples/${EXAMPLE_NAME}/*.cpp)
	else()
		message(FATAL_ERROR "No example called \"${EXAMPLE_NAME}\" found. Check the \"examples\" directory for available examples.")
	endif()
endif()

add_executable(${EXAMPLE_NAME} ${SRC_FILES})

target_include_directories(${EXAMPLE_NAME} PRIVATE)

target_link_libraries(${EXAMPLE_NAME} PRIVATE
	raylib
	flecs::flecs_static
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
	target_compile_definitions(${EXAMPLE_NAME} PRIVATE PLATFORM_DESKTOP)
endif()
