# 3DRenderer (desktop)
## Description
This program renders scenes consisting of various 3D mathematical shapes. The following shapes are available: sphere, circle, plane, rectangle, ellipse.<br/>
Shapes and the camera can be moved and rotated. Field of view of the camera can be adjusted as well.<br/>

Rendering of a single frame is done in the following way.<br/>
For every screen's pixel:
1. We cast a single ray from the camera through the pixel.
2. We check all objects on the scene and determine if the ray intersects with any of the objects.
  - If so, we take the color of the object closest to the camera from all objects, which are intersected by the ray. Then we paint the pixel with that color.
  - Otherwise, we paint the pixel with the background color (e.g. black).

The program uses std::thread to speed up frame rendering by dividing the frame into several parts and processing them simultaneously.

The program can make an animation consisting of a number of frames, which are saved to a GIF file using 'gif-h' library (https://github.com/charlietangora/gif-h).
Everything is done by the CPU without using hardware (GPU) acceleration. The program does not focus on performance but rather on the possibility of being extended or ported to other platforms.

## Building
3DRenderer is fully standalone. It only uses single header-only library 'gif-h'. Therefore, you don't need to install any dynamic-link libraries. To build 3DRenderer on Windows, firstly install an arbitrary C++ compiler i.e. MinGW-w64. Make sure that you have its 'bin' directory with 'g++.exe' file in your PATH environment variable. If you already have g++, run 'build.bat' script in Command Prompt or Powershell.

## Running
The build script creates '3DRenderer.exe' file. You can run it by specifying its path in Command Prompt or Powershell or clicking it twice in Windows File Explorer. 3DRenderer is not interactive. It means that if you want to render another scene, you must provide its description in 'main' function in 'Main.cpp' file, rebuild and then run the program again.

### Usage examples
512x512 animated GIF<br/>
![imgur; gif](https://i.imgur.com/SLsHY6a.gif)

Time taken to render the above image.<br/>
![imgur; time](https://i.imgur.com/LVlAPEX.png)
