# 3D Graphics & Simulation Projects

This repository contains three sophisticated 3D graphics and simulation programs built with modern C++. Each project demonstrates advanced computer graphics and physics techniques.

---

##  Project Overview

### 1. **Ray Tracing Renderer** (`projB05_3_final_scene`)

A photorealistic 3D ray tracing rendering engine that simulates physical light behavior to generate high-quality images.

**Key Features:**
- Monte Carlo path tracing algorithm for realistic lighting simulation
- Advanced material systems: Lambertian (matte), metallic, dielectric (glass), and emissive surfaces
- Spatial acceleration via Bounding Volume Hierarchy (BVH) for efficient ray-object intersection testing
- Volumetric effects: fog and mist rendering with scattering
- Multi-threaded tile-based rendering for parallel processing (OpenMP)
- Texture support: solid colors, checkerboard patterns, Perlin noise, and image-based textures
- Depth-of-field camera effects
- Dual output formats: PPM and PNG (with automatic image viewer launch)

**Scene Elements:**
- 400 procedurally-generated randomized boxes forming ground plane
- Multiple colored light sources (purple, red, blue, yellow)
- Glass spheres with refraction effects
- Metal spheres with reflections
- Checkerboard-textured sphere
- 1000 scattered small spheres with spatial organization

**Technology Stack:**
- C++17
- Custom PNG converter (no external image library dependencies)
- Standalone implementation of ray tracing from first principles

---

### 2. **Skeletal Animation Viewer** (`projB06_5_animate`)

An interactive 3D skeletal animation viewer for displaying rigged character models with real-time animation playback.

**Key Features:**
- Real-time skeletal animation playback system
- Support for rigged 3D models with bone hierarchies
- Interactive camera controls (rotation, zoom)
- Hierarchical scene graph node structure
- Multiple shader variants for different visual effects
- Assimp integration for flexible model loading (Collada .dae format)
- Blinn-Phong and normal mapping lighting models

**Available Shaders:**
- `blinn.vert/frag`: Blinn-Phong lighting
- `texblinn.vert/frag`: Textured Blinn-Phong
- `normalblinn.vert/frag`: Normal mapping
- `bone.vert/frag`: Skeletal hierarchy visualization (debugging)
- `colour.vert/frag`: Simple color rendering
- `flat.vert/frag`: Flat shading

**Technology Stack:**
- C++
- OpenGL (with GLAD)
- GLFW3 for window management
- Assimp for model loading
- GLM for mathematics
- CMake 3.26.0+ build system

**Customization:**
- Camera settings: light position, view center, field of view
- Window size and projection matrix
- Model loading from `bin/models/` directory
- Shader files in `bin/shaders/`

---

### 3. **Rigid Body Dynamics Simulator** (`projB09_2_rigid_simulation`)

A full-featured 3D physics simulation system that accurately models rigid body motion, collisions, and gravitational forces.

**Key Features:**
- Physics-based rigid body dynamics with linear and angular motion
- Collision detection and response: sphere-to-sphere and sphere-to-plane interactions
- Impulse-based collision resolution for realistic contact behavior
- Gravity and damping simulation
- Support for both dynamic and static rigid bodies
- Real-time 3D visualization with OpenGL
- Customizable elasticity and damping parameters
- Interactive camera controls and object manipulation

**Physics Components:**
- `RigidBody`: Base class managing position, rotation, velocity, mass, and inertia
- `RigidSphere`: Sphere-shaped bodies with specialized collision detection
- `RigidPlane`: Static infinite planes for boundaries/ground
- `RigidSim`: Main simulation engine orchestrating physics updates and collision handling

**Configurable Parameters:**
- **GRAVITY**: Global gravitational acceleration (default: -19.6133 m/s  downward)
- **LINEAR_DAMPING**: Velocity reduction per frame (0.0-1.0 scale)
- **ANGULAR_DAMPING**: Rotational damping
- **Elasticity**: Collision bounce properties

**Technology Stack:**
- C++17
- OpenGL 3.3+
- GLFW for window management
- Assimp for model loading
- GLM for mathematics
- CMake 3.26.0+ build system

---

##  Common Build Instructions

All three projects use CMake for building:

### Prerequisites
- CMake 3.26.0 or higher
- C++17 compatible compiler (MSVC recommended for Windows)
- OpenGL 3.3+
- External libraries (included in `external/` directories):
  - GLFW3
  - Assimp (for animation and simulation projects)
  - GLM (mathematics library)

### Build Steps

1. **Configure with CMake:**
   ```
   cmake -B build -G Ninja
   ```

2. **Build the project:**
   ```
   cmake --build build
   ```

3. **Run the executable:**
   - Each project generates `run01.exe` or similar in the build directory
   - Animation and Simulation projects also place executables in `bin/` directories

### Troubleshooting
- Ensure all external dependencies are properly initialized
- Verify that shader files and model assets are in the expected directories
- For animation project: confirm `assimp-vc143-mt.dll` is in the runtime directory

---

##  Directory Structure

```
 projB05_3_final_scene/          # Ray Tracing Renderer
    src/                        # Source files
    README.txt                  # Project-specific documentation
    external/glfw/              # GLFW library

 projB06_5_animate/              # Skeletal Animation Viewer
    src/                        # Source files
    bin/
       models/                 # 3D model files (.dae)
       shaders/                # GLSL shader files
       run01.exe               # Executable
    README.txt                  # Project-specific documentation
    external/
       glfw/                   # GLFW library
       assimp/                 # Assimp library
    include/                    # GLM and header files

 projB09_2_rigid_simulation/     # Rigid Body Dynamics Simulator
    src/                        # Source files
    bin/                        # Binary and asset directory
    README.txt                  # Project-specific documentation
    external/                   # External libraries
    include/                    # Header files

 README.md                        # This unified documentation
```

---

##  Use Cases

- **Ray Tracing Renderer**: Educational project in computer graphics, photorealistic image generation, demonstration of rendering algorithms
- **Animation Viewer**: Game engine pipeline testing, character animation preview, skeletal system visualization
- **Physics Simulator**: Game physics engine prototype, educational physics simulation, collision detection system testing

---

##  Technical Highlights

| Feature | Ray Tracing | Animation | Physics |
|---------|------------|-----------|---------|
| **Primary Focus** | Rendering | Animation | Simulation |
| **Main Algorithm** | Monte Carlo Path Tracing | Skeletal Interpolation | Rigid Body Dynamics |
| **Performance Optimization** | BVH Acceleration, Multi-threading | Shader Pipelines | Spatial Indexing |
| **Key Data Structures** | BVH Tree | Scene Graph | Collision Structures |
| **Rendering API** | Custom PNG | OpenGL | OpenGL |
| **Physics** | Light simulation | N/A | Full dynamics |

---

##  Notes

- Each project directory contains individual README files with detailed, project-specific information
- All projects are built with modern C++ standards and emphasize efficient algorithms
- External libraries are included to minimize setup complexity
- Projects are designed for educational purposes and can be extended with additional features

---

##  Getting Started

1. Choose a project from the three available
2. Read the individual project README.txt for specific details
3. Follow the Common Build Instructions above
4. Run the executable from the build or bin directory
5. Explore customization options detailed in each project's README

---

For detailed information about each project, see the individual `README.txt` files in each project directory.
