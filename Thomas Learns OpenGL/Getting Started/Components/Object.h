// Object.h
// header file for the Object class
// this is designed to handle the use of .obj files
#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

enum OBJECT_TYPE {
	OBJ_POINTS,
	OBJ_TRIANGLES
};

enum RENDER_TYPE {
	RENDER_POINTS,
	RENDER_LINES,
	RENDER_TRIANGLES
};

// Object class
class Object {
public:
	// struct for a vertex in a given object
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	// struct for a vertex index values
	// easily gets the position/normal/tex coords of a given vertex
	struct VertexIndex {
		int posIndex;
		int normalIndex;
		int texCoordIndex;
	};

	// struct for an individual face's index
	// holds the indices of 3 vertices in a triangle
	struct FaceIndex {
		VertexIndex vertex[3];
	};

	// VAO and VBO vectors
	std::vector<Vertex> VAOVertices;
	std::vector<unsigned int> VBOIndices;

	// original data vectors for the Object
	std::vector<glm::vec3> basePositions;
	std::vector<glm::vec3> baseNormals;
	std::vector<glm::vec2> baseTexCoords;

	// face index vector for the Object
	std::vector<FaceIndex> faces;

	// important vars

	glm::vec3 objCenter; // Object's center point
	glm::vec4 objColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f); // base Object color
	glm::mat4 objMatrix = glm::mat4(1.0f); // Object's transformation matrix

	float shininess = 32.0f;

	std::string mObjPath;
	std::string objName;

	GLuint VAO, VBO;

	OBJECT_TYPE mObjType; // determines if Object is made using points or triangles
	RENDER_TYPE mRenderType; // determines how the Object will be rendered

public:
	// functions for Object class

	//ctors
	// ctor using an Object's file path
	Object(std::string objPath);
	// ctor using a point array, useful for drawing
	Object(std::vector<glm::vec3> points);
	// dtor
	~Object();

	// load the Object from the path
	void LoadObject(std::string objPath);
	// calculate the center point of hte Object
	void CalculateCenter();
	
};


#endif