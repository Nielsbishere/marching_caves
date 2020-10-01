cmake_minimum_required(VERSION 3.13.0)
project(irmc)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE hpp "include/*.hpp")
file(GLOB_RECURSE cpp "src/*.cpp")

set(enableIgxRtTest FALSE FORCE CACHE BOOL "Enable IGX RT test")
set(enableIrmcTest TRUE CACHE BOOL "Enable irmc test")
add_subdirectory(igx_raytracing)

# Link library

add_library(
	irmc STATIC
	${hpp}
	${cpp}
	CMakeLists.txt
)

set_property(TARGET irmc PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/res")

target_include_directories(irmc PRIVATE include)
target_include_directories(irmc PRIVATE igx_raytracing/include)
target_include_directories(irmc PRIVATE igx_raytracing/igx/include)
target_include_directories(irmc PRIVATE igx_raytracing/igx/igxi-tool/igxi/include)
target_include_directories(irmc PRIVATE igx_raytracing/igx/igxi-tool/igxi/ignis/include)
target_include_directories(irmc PRIVATE igx_raytracing/igx/igxi-tool/igxi/ignis/core2/include)
target_link_libraries(irmc PRIVATE rtigx)

source_group("Headers" FILES ${hpp})
source_group("Source" FILES ${cpp})

if(MSVC)
    target_compile_options(irmc PRIVATE /W4 /WX /MD /MP /wd26812 /wd4201 /EHsc /GR)
else()
    target_compile_options(irmc PRIVATE -Wall -Wpedantic -Wextra -Werror)
endif()

if(enableIrmcTest)

	file(GLOB_RECURSE testSrc "test/*.cpp")
	file(GLOB_RECURSE testInc "test/*.hpp")

	add_executable(irmc_test ${testSrc} ${testInc})
	target_link_libraries(irmc_test PRIVATE irmc)

	target_include_directories(irmc_test PRIVATE include)
	target_include_directories(irmc_test PRIVATE igx_raytracing/include)
	target_include_directories(irmc_test PRIVATE igx_raytracing/igx/include)
	target_include_directories(irmc_test PRIVATE igx_raytracing/igx/igxi-tool/igxi/include)
	target_include_directories(irmc_test PRIVATE igx_raytracing/igx/igxi-tool/igxi/ignis/include)
	target_include_directories(irmc_test PRIVATE igx_raytracing/igx/igxi-tool/igxi/ignis/core2/include)

	add_virtual_files(
		DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/igx_raytracing/res/textures
		NAME
			textures
		FILES
			${CMAKE_CURRENT_SOURCE_DIR}/igx_raytracing/res/textures/qwantani_4k.hdr
	)

	configure_icon(irmc_test "${CMAKE_CURRENT_SOURCE_DIR}/igx_raytracing/igx/res/icon.ico")
	configure_virtual_files(irmc_test)

	if(MSVC)
	    target_compile_options(irmc_test PRIVATE /W4 /WX /MD /MP /wd26812 /wd4201 /EHsc /GR)
	else()
	    target_compile_options(irmc_test PRIVATE -Wall -Wpedantic -Wextra -Werror)
	endif()

endif()