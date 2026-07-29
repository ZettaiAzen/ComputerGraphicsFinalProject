# Projects Settings

cmake_minimum_required(VERSION 2.8...3.13)

project (csd2101+proj)

include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/glm)

add_definitions(-DGLEW_STATIC)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/glfw EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/glew EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/glfw/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/glew/include)


file(GLOB_RECURSE csd2101+proj_source_files 
    ${CMAKE_CURRENT_LIST_DIR}/projects/csd2101+proj/*.[ch]pp
    ${CMAKE_CURRENT_LIST_DIR}/projects/csd2101+proj/*.h
)
add_executable(csd2101+proj
    ${csd2101+proj_source_files}
)

target_link_directories(csd2101+proj
    PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_LIST_DIR}/lib/gfx/lib/Debug>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_LIST_DIR}/lib/gfx/lib/Release>
)
target_link_libraries(csd2101+proj
    GfxLib
    gdiplus
    glfw
    libglew_static
    opengl32
)

add_custom_command(TARGET csd2101+proj POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/projects/csd2101+proj/GfxDriver/shaders
        ${CMAKE_BINARY_DIR}/shaders
)

set_property(TARGET csd2101+proj PROPERTY CXX_STANDARD 20)
IF (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(csd2101+proj PRIVATE -Wall ${DisableWarnings})
elseif (MSVC)
    target_compile_options(csd2101+proj PRIVATE /W3 /WX-)
endif()
target_include_directories(csd2101+proj PRIVATE ${CMAKE_CURRENT_LIST_DIR}/projects/csd2101+proj/include)
target_include_directories(csd2101+proj PRIVATE ${CMAKE_CURRENT_LIST_DIR}/projects/csd2101+proj/GfxDriver/include)

