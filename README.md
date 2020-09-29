# 3DRenderer
This program renders 3D mathematical shapes. The following shapes are available: sphere, circle, plane, rectangle, ellipse.<br/>
Shapes and the camera can be moved and rotated. Field of view of the camera can be adjusted as well.<br/><br/>
Rendering of a single frame is done in the following way.<br/>
For every screen's pixel:
1. We cast a single ray from the camera through the pixel.
2. We check all objects on the scene and determine if the ray intersects any of the objects.
  - If so, we take the color of the object closest to the camera from all objects, which are intersected by the ray. Then we paint the pixel with that color.
  - Otherwise, we paint the pixel with the background color (e.g. black).
  
The program can make an animation consisting of a number of frames, which are saved to a GIF file using 'gif-h' library (https://github.com/charlietangora/gif-h).
Everything is done by the CPU and not using vertexes, polygons, shaders, etc. like in graphics libraries e.g. OpenGL.<br/>
The program does not focus on performance but rather on the possibility of being extended or ported to other platforms.<br/>
