cmake_minimum_required(VERSION 3.26.0)
cmake_minimum_required(VERSION 3.26.0)
project(proj_ray5)

# we are going to use smart pointers
set(CMAKE_CXX_STANDARD 17)

# Set output directories - executables go to bin/ folder next to src/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

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

# Copy run.bat to bin folder after build
add_custom_command(TARGET ray05 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/run.bat"
    "${CMAKE_SOURCE_DIR}/bin/run.bat"
    COMMENT "Copying run.bat to bin folder"
)

# Create output directory structure
add_custom_command(TARGET ray05 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "${CMAKE_SOURCE_DIR}/bin/out/Render Outputs"
    COMMENT "Creating output directory structure in bin folder"
)