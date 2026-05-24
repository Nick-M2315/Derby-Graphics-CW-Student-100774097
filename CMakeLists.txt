cmake_minimum_required(VERSION 3.26.0)
cmake_minimum_required(VERSION 3.26.0)
project(proj_ray5)

# we are going to use smart pointers
set(CMAKE_CXX_STANDARD 17)

# Enable optimizations for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2")

# Find and link OpenMP for multi-threading
find_package(OpenMP REQUIRED)

# specify the executable name and 
# add source files to our exectuable programs
add_executable(ray05 src/main.cpp "src/vec3.h" "src/color.h" "src/ray.h" "src/hittable.h" "src/sphere.h" "src/hittable_list.h" "src/rtweekend.h" "src/interval.h" "src/camera.h" "src/material.h" "src/aabb.h" "src/bvh.h" "src/texture.h" "src/quad.h" "src/constant_medium.h" "src/png_converter.h" "src/image_utils.h" "src/tile_renderer.h")

# Link OpenMP to the executable
target_link_libraries(ray05 PRIVATE OpenMP::OpenMP_CXX)

# Link Windows API libraries for ShellExecute
if(WIN32)
    target_link_libraries(ray05 PRIVATE shell32)
endif()