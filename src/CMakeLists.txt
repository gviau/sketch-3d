cmake_minimum_required(VERSION 2.8)

add_definitions(-DSKETCH_3D_NONCLIENT_BUILD)

add_subdirectory (math)
add_subdirectory (render)
add_subdirectory (system)
