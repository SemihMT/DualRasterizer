# Dual Rasterizer

## Overview

This C++ application allows users to switch between software rasterizing and DirectX11 rendering, providing flexibility and performance options for various use cases. The program offers several features to enhance rendering control and visualization.

## Examples

DirectX11:

<img width="474" alt="DualRasterizer_DirectX" src="https://github.com/user-attachments/assets/dfa3be6d-e72c-4533-92f4-e2e6bf1be41c">


Software:

<img width="476" alt="DualRasterizer_Software" src="https://github.com/user-attachments/assets/67429244-c214-4b9c-9caf-5d6560218340">



## Features

### General
- **Dual Rendering Modes**: Toggle between DirectX and Software rendering modes.
- **Mesh Rotation**: Enable or disable rotation of the mesh.
- **Rendering State Notifications**: Console messages indicate the current state or mode after each control is triggered.

### DirectX (Hardware) Mode
- **FireFX Mesh**: Toggle the FireFX mesh.
- **Sample States**: Cycle through different sample states (point, linear, anisotropic).
- **Cull Modes**: Cycle through backface, frontface, and no culling.
- **Uniform ClearColor**: Toggle the use of a uniform clear color.
- **FPS Printing**: Toggle the display of FPS (frames per second).

### Software Mode
- **Linear Filtering**: Enable linear filtering.
- **Shading Modes**: Cycle through different shading modes (Observed Area, Diffuse, Specular, Combined).
- **NormalMap Usage**: Toggle the usage of NormalMap.
- **DepthBuffer Visualization**: Toggle visualization of the depth buffer.
- **BoundingBox Visualization**: Toggle visualization of the bounding box.
- **Cull Modes**: Cycle through backface, frontface, and no culling.
- **Uniform ClearColor**: Toggle the use of a uniform clear color.
- **FPS Printing**: Toggle the display of FPS (frames per second).

## Controls

Below are the function keys used to control various aspects of the application:
![Screenshot 2024-07-30 181911](https://github.com/user-attachments/assets/126fa756-e9f2-4331-ad36-e8442180fd96)

- **F1**: Toggle between DirectX and Software rendering modes.
- **F2**: Enable/Disable Mesh Rotation.
- **F3**: Toggle FireFX mesh (Hardware mode only).
- **F4**: Cycle through Sample States for rendering (Anisotropic is Hardware mode only).
- **F5**: Cycle through Shading modes for rendering (Software mode only).
- **F6**: Toggle NormalMap usage (Software mode only).
- **F7**: Toggle DepthBuffer Visualization (Software mode only).
- **F8**: Toggle BoundingBox Visualization (Software mode only).
- **F9**: Cycle through Cull modes (backface, frontface, none).
- **F10**: Toggle the use of Uniform ClearColor.
- **F11**: Toggle FPS Printing.

## Usage Instructions

- Launch the application.
- Use the specified function keys to control various rendering aspects of the application.
- In **Hardware mode**, the following controls are available:
  - F1, F2, F3, F4, F9, F10, and F11.
- In **Software mode**, the following controls are available:
  - F1, F2, F5, F6, F7, F8, F9, F10, and F11.
- The console will display messages indicating the current state or mode after each control is triggered, helping you keep track of the changes.

## Additional Information

- **DirectX Initialization**: The program initializes DirectX and keeps it ready for rendering.
- **Extra Feature**: Linear filtering is available in Software mode, providing improved visual quality.

Happy Rendering!
