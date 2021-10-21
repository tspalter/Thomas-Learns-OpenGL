CS560 - Project 1

Controls:

The Camera locks the mouse into the center of the screen.  Move the mouse to change the viewpoint of the camera.
Scroll wheel = Zooms in/out from the camera's position
W = forward from the direction that the camera is facing
S = backwards from the camera direction
A = left
D = right
B = toggle bone debug drawing
M = toggle mesh polygon line debug drawing

Esc = Close the window

Interpolation algorithm is handled through the CalculateBoneTransform function in the Animator class using functions in the Bone class.  CalculateBoneTransform
recursively finds individual bones based on the ASSIMP node being checked, then calls the Bone's Update function before collecting its transformation matrix
as the node's transform.  In the Bone Update function, indiviual functions are called to interpolate the position, rotation, and scaling separately before
combining all matrices into one transformation matrix.  Note that the InterpolateRotation function calls the Slerp function from the Quaternion class, while the other
two interpolation functions simply use lerp functions; all three receive their scale factor from the same function (also in the Bone class).

Interesting bug to mention:  While it doesn't harm the rest of the program, when turning the debug bone drawing on, you notice that there is a point that
isn't attached to the model with a "moving" bone.  ASSIMP's node data collects the origin of the scene (0, 0, 0) as the first node regardless and 
therefore creates this point, trying to get the program to collect/draw all other nodes besides this first one yielded messier results while still drawing
the origin point.

Animation file used is a COLLADA (.dae) file.