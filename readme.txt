================================================================================
                    RAY TRACING RENDERER - FINAL SCENE
                         Project: RAYTRACER
================================================================================

OVERVIEW
--------
This is a sophisticated 3D ray tracing rendering engine written in C++17 that 
generates photorealistic images by simulating the physical behavior of light. 
The system traces rays from a virtual camera through a 3D scene and calculates 
realistic lighting, shadows, reflections, refractions, and volumetric effects.

The project renders a final complex scene containing:
  - 400 procedurally-generated randomized boxes forming a ground plane
  - Multiple light sources (purple, red, blue, and yellow)
  - Glass spheres with refraction effects
  - Metal spheres with realistic reflections
  - Fog/volumetric scattering effects
  - Checkerboard-textured sphere
  - 1000 randomly scattered small spheres with spatial organization

Output is saved as both PPM (Portable PixMap) and PNG formats, with the PNG
automatically opened in the default image viewer after rendering completes.


SYSTEM COMPONENTS
-----------------

Core Ray Tracing Engine:
  - ray.h: Represents a ray in 3D space (origin and direction)
  - vec3.h: 3D vector mathematics (points, vectors, operations)
  - color.h: Color representation and operations
  - camera.h: Virtual camera with perspective projection, depth-of-field,
             and tiled rendering for parallel processing

Scene Representation:
  - hittable.h: Abstract base class for all renderable objects
  - hittable_list.h: Container for multiple hittable objects
  - sphere.h: Spherical geometry with material assignment
  - quad.h: Quadrilateral surfaces for light sources and walls
  - box(): Helper function for creating axis-aligned boxes

Spatial Optimization:
  - aabb.h: Axis-Aligned Bounding Box for quick ray-object intersection tests
  - bvh.h: Bounding Volume Hierarchy - recursive tree structure that 
          dramatically accelerates ray casting by pruning non-intersecting branches

Materials & Surface Properties:
  - material.h: Base material class with light emission and scattering
  - lambertian: Matte, diffuse surfaces with Lambertian reflection
  - metal: Reflective surfaces with adjustable fuzz/roughness
  - dielectric: Transparent/translucent materials with refraction (glass, water)
  - diffuse_light: Light-emitting surfaces
  - isotropic: Phase function for volumetric scattering in fog/mist

Textures:
  - texture.h: Base texture class
  - solid_color: Uniform color texture
  - checker_texture: Checkerboard pattern texture
  - image_texture: Image-based textures (supports JPEG via external library)
  - noise_texture: Perlin noise for procedural textures

Volumetric Effects:
  - constant_medium.h: Implements fog, mist, and volume scattering effects
                       by randomly determining whether rays scatter within
                       a bounded volume

Performance Features:
  - tile_renderer.h: Divides image into tiles (32x32 default) for parallel
                    processing using OpenMP multi-threading
  - tile_statistics.h: Tracks rendering progress and performance metrics
  - BVH acceleration structure: Reduces ray-scene intersection time from O(n)
                               to O(log n)

Output & Utilities:
  - png_converter.h: Standalone PNG writer that converts PPM to PNG without
                    external library dependencies. Handles CRC checksums,
                    PNG chunk structure, and DEFLATE compression.
  - image_utils.h: Coordinates PPM to PNG conversion and image viewing
  - main.cpp: Orchestrates scene creation, rendering, and output


RENDERING ALGORITHM
-------------------
The renderer uses a Monte Carlo path tracing approach:

1. For each pixel in the image:
   - Cast multiple sample rays through the pixel (samples_per_pixel)
   - Each ray follows random bounces until it hits a light source or max depth

2. For each ray bounce:
   - Use BVH to find nearest scene geometry
   - Evaluate material at hit point (Lambertian, metal, glass, etc.)
   - Sample scattered ray direction based on material properties
   - Recursively trace the scattered ray (limited depth to prevent infinite loops)

3. Accumulate color contributions and average over all samples
   - More samples = less noise but longer render time
   - Depth limit prevents infinite recursion but can lose light information

4. Render in parallel using tile-based approach:
   - Scene divided into 32x32 pixel tiles
   - Each thread processes complete tiles independently
   - Reduces synchronization overhead and improves cache locality


HOW TO RUN
----------

Prerequisites:
  - Windows 7 SP1 or later (for already-built executable)
  - 4 GB RAM minimum
  - CPU supporting AVX2 instructions (Intel Haswell+ or AMD Steamroller+)
  - For building: CMake 3.26.0+, C++17 compiler, OpenMP, Visual Studio build tools

Running the Pre-Built Executable (Easiest):
  1. Navigate to: bin/ folder
  2. Double-click: bin/ray05.exe
  3. Wait for rendering to complete (1-10 minutes depending on settings)
  4. View results:
     - PNG opens automatically in default image viewer
     - Or view at: bin/out/Render Outputs/render_output.png
  5. Optional: View PPM in browser by double-clicking bin/run.bat

Running from Command Line:
  1. Open Command Prompt
  2. Navigate to project root: cd C:\path\to\projB05_3_final_scene
  3. Run directly: bin\ray05.exe
  4. Or with relative path from bin: cd bin && ray05.exe

Running After Building with build_release.bat:
  Quick Start (Recommended):
    1. Double-click build_release.bat in project root
    2. Wait for build to complete (~30 seconds)
    3. Run: bin/ray05.exe
    4. Rendered images appear in: bin/out/Render Outputs/

  Manual Build Instructions:
    1. Navigate to project root directory
    2. Create build directory: mkdir build_release && cd build_release
    3. Configure with CMake: cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
    4. Build: cmake --build . --config Release
    5. Run executable: ..\bin\ray05.exe

Output Files:
  Location: bin/out/Render Outputs/
  - render_output.ppm: PPM format image (raw, larger file)
  - render_output.png: PNG format image (compressed, auto-converted from PPM)
  - Image automatically opens in default viewer on Windows
  - If not, manually open render_output.png from the Render Outputs folder

Console Output During Rendering:
  - Real-time progress information
  - Timing for rendering stages
  - File paths where output is saved
  - Completion notification

Viewing PPM Output in Web Browser:
  The bin/ folder includes a run.bat file for viewing PPM in a web-based viewer:

  1. After rendering completes and output files are generated in bin/out/Render Outputs/
  2. Option A - Direct: Double-click bin/run.bat
  3. Option B - Manual: Open browser and navigate to https://www.cs.rhodes.edu/welshc/COMP141_F16/ppmReader.html
  4. Click "Choose File" and navigate to bin/out/Render Outputs/render_output.ppm
  5. The PPM image displays in the browser for viewing and analysis

  This is useful for:
    - Quick preview without installing image viewers
    - Comparing render quality without conversion
    - Viewing on different systems via file sharing
    - Analyzing image details at full resolution

About the bin/ Folder:
  - bin/ is the main distribution folder containing all executable files
  - Location: projB05_3_final_scene/bin/ (next to src/ folder)
  - Self-contained package: ray05.exe, run.bat, and output directory
  - Can be packaged and distributed to other systems
  - All files stay within bin/ hierarchy for portability
  - No external dependencies - everything included
  - See BUILD_INSTRUCTIONS.txt for detailed setup and distribution guide


CUSTOMIZATION GUIDE
-------------------

Most Important Customization Points:

1. RENDER QUALITY (in main.cpp, final_scene function call):
   Current: final_scene(512, 500, 10);  // ~7 minutes

   Parameters:
   - image_width: Output image resolution (512 = 512x512 pixels)
   - samples_per_pixel: Rays per pixel (higher = less noise, slower render)
   - max_depth: Maximum ray bounce depth (higher = better quality, slower)

   Quick render (debug):
     final_scene(256, 100, 4);    // ~20 seconds

   Production quality:
     final_scene(512, 1000, 40);  // ~15 minutes

   High quality (very slow):
     final_scene(2048, 2000, 40); // Several hours

2. CAMERA SETTINGS (in final_scene function, camera section):
   - vfov: Vertical field of view in degrees (40 = current, 90 = wide)
   - lookfrom: Camera position point3(478, 278, -600)
   - lookat: What the camera points at point3(278, 278, 0)
   - vup: Up direction vec3(0, 1, 0) (always 0, 1, 0 for normal orientation)
   - defocus_angle: Controls depth-of-field blur (0 = no blur)
   - exposure: Brightness multiplier (1.0 = normal, <1.0 = darker, >1.0 = brighter)

3. LIGHT SOURCES (in final_scene function, light section):
   Add/modify lights by creating quadrilateral surfaces with diffuse_light material:

   Example - Purple light at (123, 554, 147) with size 300x265:
     auto light = make_shared<diffuse_light>(color(80, 20, 80));
     world.add(make_shared<quad>(point3(123, 554, 147), 
                                 vec3(300, 0, 0), 
                                 vec3(0, 0, 265), 
                                 light));

   Modify existing lights by changing:
   - Position: point3(x, y, z) values
   - Size: vec3 parameters (vectors defining edges)
   - Intensity: color RGB values (larger = brighter)

4. SCENE OBJECTS:
   Add spheres:
     world.add(make_shared<sphere>(point3(x, y, z), radius, material));

   Material options:
     - make_shared<lambertian>(color(r, g, b)): Matte diffuse
     - make_shared<metal>(color(r, g, b), fuzz): Reflective (fuzz 0-1)
     - make_shared<dielectric>(ior): Glass (ior ~1.5 for glass)
     - make_shared<diffuse_light>(color(r, g, b)): Emissive

5. VOLUMETRIC EFFECTS (fog/mist):
   Add fog inside a sphere boundary:
     auto boundary = make_shared<sphere>(point3(x, y, z), radius, 
                                         make_shared<dielectric>(1.5));
     world.add(boundary);
     world.add(make_shared<constant_medium>(boundary, density, color(r, g, b)));

   - density: Controls how thick/opaque fog is (0.1 = very light, 0.01 = very dense)
   - color: Color of the fog/mist

6. GROUND PLANE:
   Current: 20x20 grid of random-height boxes, each 100x100 units
   To modify dimensions (in final_scene, boxes1 section):
     int boxes_per_side = 20;  // Change to 10 or 30 for different scales
     auto w = 100.0;           // Box width (smaller = more boxes)

7. TILE SIZE (for parallel rendering):
   Edit in camera.h, render_tiled() call:
     cam.render_tiled(world, 32);  // Tile size in pixels
   - Smaller tiles (16) = more parallelism but more overhead
   - Larger tiles (64) = less overhead but less parallel efficiency

8. BACKGROUND COLOR:
   In final_scene, camera section:
     cam.background = color(0, 0, 0);  // Change RGB to set background
   Current is black (0, 0, 0). Use color(0.7, 0.8, 1.0) for sky blue.


PERFORMANCE OPTIMIZATION TIPS
------------------------------

1. Use BVH (Bounding Volume Hierarchy):
   - Already enabled by default for all objects
   - Makes render time scale ~log(n) instead of O(n) where n = object count
   - Essential for scenes with thousands of objects

2. Tile Rendering Parallelization:
   - Current: 32x32 tile size works well for most systems
   - System automatically uses all available CPU cores via OpenMP
   - Monitor CPU usage; if not at 100%, try smaller tile size

3. Sample Count vs. Depth Trade-off:
   - samples_per_pixel: Controls noise (primary factor for quality)
   - max_depth: Controls light bounce depth (secondary factor)
   - Usually: reduce depth before samples if optimizing time

4. Adaptive Sampling:
   - Consider implementing per-pixel sample count based on variance
   - Not currently in codebase but would improve convergence

5. Release Builds:
   - Build with Release configuration: cmake --build . --config Release
   - Includes /O2 optimization and AVX2 vector instructions
   - Can be 10-100x faster than Debug builds


FILE STRUCTURE
--------------
projB05_3_final_scene/
??? CMakeLists.txt               - Build configuration (outputs to bin/)
??? build_release.bat            - Quick build script (creates bin/ folder)
??? BUILD_INSTRUCTIONS.txt       - Detailed build and distribution guide
??? readme.txt                   - This file
??? run.bat                      - PPM browser viewer launcher
??? src/
?   ??? main.cpp                - Entry point and scene definition
?   ??? camera.h                - Camera and rendering orchestration
?   ??? ray.h                   - Ray primitive
?   ??? vec3.h                  - 3D vector math
?   ??? color.h                 - Color handling
?   ??? hittable.h              - Renderable object interface
?   ??? hittable_list.h         - Object container
?   ??? sphere.h                - Sphere geometry
?   ??? quad.h                  - Quadrilateral geometry
?   ??? material.h              - Material definitions (lambertian, metal, etc.)
?   ??? texture.h               - Texture definitions
?   ??? bvh.h                   - Acceleration structure
?   ??? aabb.h                  - Bounding box
?   ??? constant_medium.h       - Volumetric scattering
?   ??? tile_renderer.h         - Tile-based parallel rendering
?   ??? png_converter.h         - PNG output writer
?   ??? image_utils.h           - Image conversion and viewing
?   ??? rtweekend.h             - Common utilities and includes
??? bin/                         - Distribution folder (created by build)
?   ??? ray05.exe               - Release executable (optimized)
?   ??? run.bat                 - PPM viewer launcher (copied from root)
?   ??? out/
?       ??? Render Outputs/     - Output directory for renders
??? build_release/              - Build directory (created by build_release.bat)
??? CMake build files


TECHNICAL DETAILS
-----------------

Coordinate System:
  - Right-handed coordinate system: X (right), Y (up), Z (back)
  - Camera origin typically at (478, 278, -600)
  - Scene typically centered around (278, 278, 0)

Units:
  - All measurements in world units
  - No implicit unit (could be cm, m, arbitrary)
  - Scale is relative; only proportions matter

Material Physics:
  - Lambertian: Diffuse reflection following cosine-weighted hemisphere
  - Metal: Specular reflection with optional fuzz (surface roughness)
  - Dielectric: Fresnel refraction with proper IOR handling
  - Isotropic: Uniform scattering in all directions (for fog)

Color Space:
  - sRGB linear color space (no gamma correction in this version)
  - Values can exceed 1.0 (HDR) before tone-mapping
  - PNG output clamps to 0-255 range

Multi-threading:
  - Uses OpenMP for automatic parallelization
  - Each tile rendered independently on separate thread
  - Controlled by OMP_NUM_THREADS environment variable


TROUBLESHOOTING
---------------

Image too dark:
  - Increase cam.exposure value (try 0.5, 1.0, 2.0)
  - Increase light source intensity values
  - Increase samples_per_pixel

Image too noisy:
  - Increase samples_per_pixel (512, 1000, 2000)
  - Reduce max_depth if render time is acceptable

Render takes too long:
  - Reduce image_width (try 256 instead of 512)
  - Reduce samples_per_pixel (try 100 instead of 500)
  - Reduce max_depth (try 4 instead of 10)
  - Use Release build configuration

Render crashes or runs out of memory:
  - Reduce image_width significantly (256 or lower)
  - Reduce samples_per_pixel
  - Simplify scene (fewer objects)

PNG conversion fails:
  - Check file permissions in out/Render Outputs directory
  - Verify disk has adequate free space
  - Check that intermediate PPM file was created

Image doesn't open automatically:
  - Run on Windows (uses ShellExecute API)
  - Check that default image viewer is configured
  - PNG file should still be created even if open fails


================================================================================
For further information, examine the source code comments marked with
>>>> and <<<< arrows, which highlight recent modifications and enhancements.
================================================================================
