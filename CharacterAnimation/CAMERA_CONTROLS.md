# Modern Camera Control System - Unreal Engine 5 Style

## Overview
The project now features a modern camera control system inspired by Unreal Engine 5, providing intuitive first-person flying controls with mouse-based camera aiming.

## Camera Controls

### Keyboard Controls (WASD Flying)
- **W** - Move camera forward in the direction it's facing
- **S** - Move camera backward (reverse)
- **A** - Move camera left (strafe)
- **D** - Move camera right (strafe)
- **SPACE** - Move camera up (ascend)
- **LEFT CTRL** - Move camera down (descend)
- **R** - Reset camera to initial position and orientation

### Mouse Controls

#### Camera Aiming (Right-Click + Drag)
- **Right-click and drag** - Aim/rotate the camera following mouse movement
  - Move mouse horizontally to pan left/right (Yaw)
  - Move mouse vertically to tilt up/down (Pitch)
  - Pitch is clamped between -89° and +89° to prevent camera flip

#### Mouse Scroll Wheel (Acceleration Control)
- **Scroll Up** - Increase camera fly speed (acceleration multiplier increases)
- **Scroll Down** - Decrease camera fly speed (acceleration multiplier decreases)
- Acceleration is clamped between 0.5x and 5.0x
- Default acceleration is 1.0x

### Model Manipulation (Separate from Camera)
- **CTRL + Arrow Keys** - Translate the model in world space
  - LEFT / RIGHT - Move left/right
  - UP / DOWN - Move up/down

## Implementation Details

### Camera Class (`src/Camera.h`)
The `Camera` class handles all camera-related operations:
- **Position** - Current 3D position in world space
- **Front** - Direction vector the camera is looking
- **Up** - Up vector for camera orientation
- **Right** - Right vector for camera orientation
- **Yaw** - Rotation around the Y-axis (horizontal)
- **Pitch** - Rotation around the X-axis (vertical)
- **MovementSpeed** - Base speed of camera movement (50 units/second)
- **Acceleration** - Speed multiplier controlled by scroll wheel (0.5x to 5.0x, default 1.0x)
- **MouseSensitivity** - Sensitivity of mouse input (0.5 degrees per pixel)
- **Zoom** - Field of View angle (fixed at 70°)

### Key Functions
- `GetViewMatrix()` - Returns the view matrix for rendering
- `ProcessKeyboard(direction, deltaTime)` - Handle WASD movement (applies acceleration multiplier)
- `ProcessMouseMovement(xoffset, yoffset)` - Handle mouse aiming
- `ProcessMouseScroll(yoffset)` - Handle scroll wheel acceleration control
- `SetMovementSpeed(speed)` - Adjust camera base movement speed
- `GetZoom()` - Get current FOV
- `GetAcceleration()` - Get current acceleration multiplier

### Integration Points
1. **Mouse Movement** - `mouse_callback()` in main.cpp tracks mouse position and calls `ProcessMouseMovement()`
2. **Mouse Buttons** - `mouse_button_callback()` detects right-click press/release
3. **Mouse Scroll** - `scroll_callback()` handles scroll wheel input and updates projection matrix
4. **Keyboard** - Main loop checks for WASD keys and calls `ProcessKeyboard()` with delta time
5. **View Matrix** - Updated each frame: `matView = camera->GetViewMatrix()`

## Configuration

To adjust camera behavior, modify these values in `Camera.h` constructor or in `main.cpp`:
- **Base Movement Speed**: Change `MovementSpeed` in Camera constructor (currently 50.0f units/second)
- **Mouse Sensitivity**: Modify `MouseSensitivity` in Camera constructor (currently 0.5f degrees per pixel)
- **Acceleration Limits**: Change the clamp values in `ProcessMouseScroll()` (min 0.5x, max 5.0x)
- **Initial Position**: Change the first parameter in the Camera constructor
- **Initial Orientation**: Adjust Yaw and Pitch in the Camera constructor

## Features

? Smooth flying camera (first-person)
? WASD continuous movement with delta time scaling
? Right-click drag for camera aiming (increased mouse sensitivity)
? Mouse scroll wheel for speed acceleration control (0.5x to 5.0x multiplier)
? Base flying speed increased 5x (50 units/second)
? Pitch and Yaw constraints to prevent gimbal lock
? Euler angle-based camera system (similar to Unreal Engine)
? Separate model manipulation controls
? Delta time-based movement (framerate-independent)
? Acceleration affects all movement directions consistently
