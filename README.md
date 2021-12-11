CS560 - Project 3

To Compile:  Run the .sln file in VS 2019

CONTROLS
=================
Camera:
W = Forward
S = Backwards
A = Left
D = Right
Left Shift = Down
Space = Up

Target Position (Green Cube) Controls:
I = Up
K = Down
J = Left
L = Right
U = Forwards
O = Backwards

Simulation:
Enter = Run the Simulation



Algorithms/Important Files

This program implements inverse kinematics through a Jacobian transpose matrix.  You will find the logic for writing this code in the IKBoneAnimation.cpp file, in the
SetJacobian, GetTranspose, InverseKinematics, and finally ComputeAngles functions.  These functions set up the Jacobian matrix, get the transpose of it, implement the necessary
transformations to each bone individually, and finally compile and update the links in the chain to allow the end effector to reach for the target object.


Note:  I tried to get this system to work with 5 links in the chain, but I only managed to get 3 working and ran out of time trying to fix the bugs.  Regardless, thank you so
much for being available and helping me, and I'm proud of myself for getting this to work as intended!
