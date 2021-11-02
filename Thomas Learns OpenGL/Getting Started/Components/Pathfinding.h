#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <glad/glad.h>
#include <vector>

#include "Shader.h"

typedef std::vector<std::pair<float, float>> tArcValues;

// struct for a simple 3D point
struct Point3D {
	float x, y, z;
};

// function to find the spline point based on the concatenated t-value
Point3D GetSpline(std::vector<Point3D> points, float t);

// function to take spline points and use them to determine the arc length table, 
// using approximation by forward differencing
std::vector<float> GetArcLengths(std::vector<Point3D> splinePoints);

// get the final point values based on the curve length
tArcValues GetFinalPairValues(std::vector<Point3D> points, std::vector<float> splinePoints, float curveLength);

// binary search for a given arc length based on the table
float FindArcLength(tArcValues arcValuePairs, int l, int r, float t);

// binary search for the index provided an arc length
int FindArcIndex(tArcValues arcValuePairs, int l, int r, float arcLength);

// find the given parametric value of a given arc length (u = G^-1(s))
float FindParametricValue(tArcValues arcValuePairs, float t, float dT);

// distance formula between two vec3
float GetDistance(glm::vec3 p1, glm::vec3 p2);

// function to draw control points of the line based on the points provided
void DrawPath(Shader& shader, std::vector<float> pointCoords);
// draws the path based on the splines
void DrawSplinePath(Shader& shader, std::vector<float> pointCoords);

// converts the points vector into a vector of floats so that it can be read for drawing
std::vector<float> ConvertPointsToFloats(std::vector<Point3D> points);

#endif