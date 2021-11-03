CS560 - Project 2

Controls:

The Camera locks the mouse into the center of the screen.  Move the mouse to change the viewpoint of the camera.
Scroll wheel = Zooms in/out from the camera's position
W = forward from the direction that the camera is facing
S = backwards from the camera direction
A = left
D = right
B = toggle bone debug drawing
M = toggle mesh polygon line debug drawing
P = toggle path debug drawing
Left Arrow = Increase speed of the model
Right Arrow = Decrease speed of the model (default set to slowest speed)

Esc = Close the window

For this implementation, I ended up using the approximation by forward
differencing.  I found that while there are several issues with the way the
approximation works, it provided a satisfying pathfinding algorithm.  To 
implement this, I created a separate file for all Pathfinding equations.
Included are a binary search that finds the index of a section of the arc, 
a calculation to properly determine the location of spline points based on the
provided control points, and a calculation of the arc lengths for each given
section of the path based on those spline points.  In the main method, I was
able to iterate through different speeds and easing on the animations based 
on the location on the path (the character animation is slower where arc lengths
are shorter and faster on longer stretches).

