================================================================================
RIGID BODY DYNAMICS SIMULATION SYSTEM
================================================================================

DESCRIPTION
============

This is a 3D rigid body dynamics simulation system built with OpenGL and C++. 
The system simulates the physical behavior of rigid bodies (such as spheres and 
planes) under gravity and collision forces. It features:

- Physics-based rigid body dynamics with linear and angular motion
- Collision detection between rigid bodies (sphere-to-sphere and sphere-to-plane)
- Impulse-based collision response for realistic interactions
- Interactive 3D visualization using OpenGL
- Support for dynamic and static rigid bodies
- Customizable damping, gravity, and elasticity parameters


MAIN COMPONENTS
================

1. RigidBody System
   - RigidBody.h/cpp: Base class for all rigid bodies
   - Manages position, rotation, velocity, acceleration, and mass
   - Handles force and impulse application
   - Integrates linear and angular motion

2. Rigid Body Types
   - RigidSphere.h/cpp: Sphere-shaped rigid bodies with collision detection
   - RigidPlane.h/cpp: Infinite plane for ground/boundaries
   - Specialized collision testing for each shape type

3. Physics Simulation Engine
   - RigidSim.h/cpp: Main simulation engine
   - Manages list of rigid bodies
   - Executes physics updates each frame (force application, velocity integration)
   - Detects collisions and computes collision responses
   - Applies gravity and damping forces

4. Rendering System
   - SphereMesh.h/cpp: Mesh geometry for sphere visualization
   - PlaneMesh.h/cpp: Mesh geometry for plane visualization
   - shader.cpp: OpenGL shader compilation and management
   - Texture.h/cpp: Texture loading and application
   - GLWin.h: OpenGL window management via GLFW

5. Interaction System
   - Interaction.h: User input handling
   - Camera controls and object manipulation

6. Supporting Components
   - Spatial.h/cpp: Spatial data structures for collision detection
   - Mesh.h: Base mesh class for rendering
   - MeshFactory.h: Factory for creating mesh objects
   - AppMain.h: Global application state management


HOW TO RUN
===========

Prerequisites:
- CMake 3.26.0 or higher
- C++17 compatible compiler (MSVC on Windows)
- OpenGL 3.3+
- Dependencies: GLFW, OpenGL, ASSIMP

Build Instructions:
1. Open CMake GUI or command line
2. Set source directory to the project root
3. Set build directory to a subdirectory (e.g., "build")
4. Configure with generator: Ninja
5. Generate build files
6. Build:
   - Command line: ninja
   - Or use your IDE to build the project

Run the Application:
1. After build, locate the executable "run01.exe" in the build directory
2. Double-click to run, or execute from command line:
   > run01.exe

3. The application will open a window showing the 3D simulation


HOW TO CUSTOMIZE
=================

Physics Parameters (Edit in RigidSim.h):
- GRAVITY: Modify the global gravity vector (default: -19.6133 m/s² downward)
  const glm::vec3 GRAVITY = glm::vec3(0.0f, -19.6133, 0.0f);

- LINEAR_DAMPING: Control how much linear velocity is reduced each frame
  const float LINEAR_DAMPING = 0.999;  (1.0 = no damping, 0.0 = instant stop)

- ANGULAR_DAMPING: Control rotational damping
  const float ANGULAR_DAMPING = 0.999;

Collision Parameters (Edit in RigidBody.h):
- elasity: Controls bounce of collisions
  const float elasity = 0.95f;  (0.0 = no bounce, 1.0 = perfect bounce)

Object Properties (Edit in main.cpp or runtime):
- Mass: Set via RigidBody::setMass(float m)
- Position: Set via RigidBody::setPosition(glm::vec3 pos)
- Initial Velocity: Set via RigidBody::setVelocity(glm::vec3 vel)
- Dynamic/Static: Set via RigidBody::setDynamic(bool b)
- Use Gravity: Set via RigidBody::setUseGravity(bool b)

Scale and Appearance:
- Object scale: Modify the scale member of RigidBody
- Mesh vertices: Edit SphereMesh.cpp for sphere resolution
- Colors: Modify shader files in the shaders/ directory

Scene Setup:
- Add objects to simulation: rigidSim.add(rigidBodyObject)
- Remove all objects: rigidSim.clear()
- Toggle simulation playback: rigidSim.setPlaySim(bool b)
- Toggle angular motion: rigidSim.setUseAngular(bool b)


SIMULATION WORKFLOW (Physics Loop)
===================================

Each frame (in RigidSim::tick()):
1. Apply gravity force to dynamic bodies using Gravity = mass * GRAVITY
2. Integrate accelerations to update velocities: v += a * dt
3. Collision detection: Test all body pairs for intersection
4. Collision response: Apply impulses to resolve collisions
5. Apply linear/angular damping
6. Integrate velocities to update positions: pos += v * dt


INPUT CONTROLS
===============

The Interaction system handles user input (details in Interaction.h):
- Camera movement and rotation
- Object selection and manipulation
- Play/pause simulation
- Enable/disable angular motion


DIRECTORY STRUCTURE
====================

projB09_2_rigid_simulation/
??? src/                    # Source files
?   ??? main.cpp           # Application entry point
?   ??? RigidSim.h/cpp     # Simulation engine
?   ??? RigidBody.h/cpp    # Base rigid body class
?   ??? RigidSphere.h/cpp  # Sphere rigid body
?   ??? RigidPlane.h       # Plane rigid body
?   ??? SphereMesh.h/cpp   # Sphere mesh
?   ??? PlaneMesh.h/cpp    # Plane mesh
?   ??? Spatial.h/cpp      # Spatial structures
?   ??? GLWin.h            # OpenGL window
?   ??? Interaction.h      # Input handling
?   ??? shader.cpp         # Shader management
?   ??? Texture.h/cpp      # Texture loading
?   ??? Mesh.h             # Base mesh class
?   ??? MeshFactory.h      # Mesh factory
?   ??? AppMain.h          # Global state
??? include/               # Header files
??? shaders/               # GLSL shader files
??? models/                # 3D model files
??? external/              # External libraries (GLFW, ASSIMP, GLAD)
??? CMakeLists.txt         # CMake configuration
??? README.txt             # This file


TROUBLESHOOTING
================

Build Issues:
- Ensure CMake version is 3.26.0 or higher
- Verify all dependencies are installed
- Check that ASSIMP library path is correct in CMakeLists.txt
- For Windows: Ensure Visual Studio 2022 and Ninja are installed

Runtime Issues:
- If shaders don't load: Verify shaders/ directory is copied to build output
- If models don't load: Check models/ directory exists and paths are correct
- Window won't open: Check GLFW installation and OpenGL 3.3+ support


NOTES
======

- This is an educational implementation of rigid body dynamics
- The system uses Euler integration for physics (simple but less accurate)
- Collision detection uses sphere-sphere and sphere-plane algorithms
- The simulation assumes rigid bodies (no deformation)
- Performance scales linearly with number of objects (O(n²) collision checks)

================================================================================
