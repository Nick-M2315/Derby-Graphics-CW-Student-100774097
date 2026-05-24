===============================================================================
SKELETAL ANIMATION VIEWER
===============================================================================

OVERVIEW
--------
This program is a 3D skeletal animation viewer built with C++ using OpenGL.
It loads rigged 3D models with bone skeletal systems and can playback 
animations in real-time. The application supports model loading via Assimp
and provides interactive camera controls for viewing animations from different
angles.

COMPONENTS
----------
1. Core Rendering
   - Mesh.h/cpp: Handles mesh data, vertices, and skeletal bone influences
   - shader.cpp/h: OpenGL shader compilation and management
   - Node.h/cpp: Hierarchical scene graph node structure
   - Camera.h: Interactive camera system for viewing control

2. Animation System
   - Animation.h: Core animation playback engine
   - Animator.h: Animation controller and timing
   - Bone.h: Bone data structure and skeletal hierarchy

3. Resource Management
   - External libraries: OpenGL (GLAD), GLFW3, Assimp (for model loading), GLM (math)
   - bin/shaders/: GLSL shader files (vertex and fragment)
   - bin/models/: Sample 3D model files (DAE format)
   - Assimp DLL: Runtime dependency for model loading

RUNNING THE PROGRAM
-------------------
1. Build the project using CMake:
   - Requires CMake 3.26.0 or higher
   - Uses Ninja generator

2. From the bin/ directory:
   - The executable (run01.exe) and all required resources are in bin/
   - Run the executable:
     ./run01.exe

3. The program requires:
   - assimp-vc143-mt.dll (automatically copied during build)
   - shaders/ subdirectory (vertex and fragment shader files)
   - models/ subdirectory (3D model files in DAE format)

CUSTOMIZATION
-------------
1. Model Loading:
   - Models are loaded from bin/models/ directory
   - Supported format: Collada (.dae) via Assimp
   - Add new models to bin/models/ and modify main.cpp to load them

2. Shaders:
   - Located in bin/shaders/ directory
   - Available shader variants:
     * blinn.vert/frag: Blinn-Phong lighting
     * texblinn.vert/frag: Textured Blinn-Phong
     * normalblinn.vert/frag: Normal mapping
     * bone.vert/frag: Bone visualization (for debugging skeletal hierarchy)
     * colour.vert/frag: Simple color rendering
     * flat.vert/frag: Flat shading
   - Modify shader files to change material appearance or add effects

3. Camera Settings (main.cpp):
   - Light position: lightPos vector
   - View position: viewPos vector
   - View center: viewCenter vector
   - Field of view: fov variable
   - Near/far planes: near and far variables

4. Animation Playback:
   - Toggle animation with 'animate' variable
   - Modify deltaTime calculation to change animation speed
   - Adjust animation playback in Animator.h

5. Display Settings:
   - Window size: wView (800) and hView (800) variables
   - Projection matrix: matProj (currently orthographic, can change to perspective)
   - Model scale: scale vector

KEY CONTROLS (typical implementation)
-------------------------------------
- Right Mouse Button + Movement: Rotate camera
- Mouse Scroll: Zoom camera in/out
- Keyboard: Animation playback control (depends on implementation)

BUILD DEPENDENCIES
------------------
- OpenGL (system)
- GLFW3 (external/)
- Assimp (external/assimp/)
- GLM (include/)

For additional details, see CMakeLists.txt for build configuration.
===============================================================================
