// Object.cpp
// source file for the Object class
#include "Object.h"

Object::Object(std::string objPath) {
	mObjPath = objPath;
	LoadObject(mObjPath); // load the object from the file path
	CalculateCenter(); // calculate the object's center
	// set object types
	mObjType = OBJ_TRIANGLES;
	mRenderType = RENDER_TRIANGLES;
}

Object::Object(std::vector<glm::vec3> points) {
	// make sure vectors are cleared
	VAOVertices.clear();
	VBOIndices.clear();
	faces.clear();

	basePositions.clear();
	baseNormals.clear();
	baseTexCoords.clear();

	// now create the object from the points
	for (unsigned int i = 0; i < points.size(); i++) {
		// add point to positions
		glm::vec3 currPoint = points[i];
		basePositions.push_back(currPoint);
		// add vertex to VAO vector
		Vertex currVert;
		currVert.position = currPoint;
		VAOVertices.push_back(currVert);
	}
	CalculateCenter();
	// set object types
	mObjType = OBJ_POINTS;
	mRenderType = RENDER_POINTS;
}

Object::~Object() { }

void Object::LoadObject(std::string objPath) {
	// check if path yields a .obj file
	int pathLength = objPath.size();
	std::string suffix = objPath.substr(pathLength - 3, pathLength);

	if (suffix == "obj") {
		// .obj file found, clear vectors
		VAOVertices.clear();
		VBOIndices.clear();
		faces.clear();

		basePositions.clear();
		baseNormals.clear();
		baseTexCoords.clear();

		std::ifstream ifs;

		// store the original data vector
		try {
			ifs.open(objPath);
			std::string line;
			while (getline(ifs, line)) {
				std::stringstream ss(line);
				std::string type;
				ss >> type;
				if (type == "v") {
					// vertex position
					glm::vec3 vertPos;
					ss >> vertPos[0] >> vertPos[1] >> vertPos[2];
					basePositions.push_back(vertPos);
				}
				else if (type == "vt") {
					// texture coordinates
					glm::vec2 texCoord;
					ss >> texCoord[0] >> texCoord[1];
					baseTexCoords.push_back(texCoord);
				}
				else if (type == "vn") {
					// normals
					glm::vec3 vertNorm;
					ss >> vertNorm[0] >> vertNorm[1] >> vertNorm[2];
					baseNormals.push_back(vertNorm);
				}
				else if (type == "f") {
					// face index
					FaceIndex faceIndex;
					// only push back face 3
					for (int i = 0; i < 3; i++) {
						std::string sVertex;
						ss >> sVertex;
						int posIndex = -1;
						int texIndex = -1;
						int normIndex = -1;
						sscanf(sVertex.c_str(), "%d/%d/%d", &posIndex, &texIndex, &normIndex);
						// We have to use index -1 because the obj index starts at 1
						// Incorrect input will be set as -1
						faceIndex.vertex[i].posIndex = posIndex > 0 ? posIndex - 1 : -1;
						faceIndex.vertex[i].texCoordIndex = texIndex > 0 ? texIndex - 1 : -1;
						faceIndex.vertex[i].normalIndex = normIndex > 0 ? normIndex - 1 : -1;
					}
					faces.push_back(faceIndex);
				}
			}
		}
		catch (const std::exception&) {
			std::cout << "Error: Object file cannot be opened" << std::endl;
		}

		// Retrieve data from the index and assign values to VAO and VBO
		for (int i = 0; i < faces.size(); i++) {
			FaceIndex currFace = faces[i];
			// if there is no normal, re-calculate for it
			glm::vec3 v0 = basePositions[currFace.vertex[0].posIndex];
			glm::vec3 v1 = basePositions[currFace.vertex[1].posIndex];
			glm::vec3 v2 = basePositions[currFace.vertex[2].posIndex];
			glm::vec3 newNorm = glm::cross(v1 - v0, v2 - v0);

			for (int j = 0; j < 3; j++) {
				Vertex currVert;
				VertexIndex currVertIndex = currFace.vertex[j];
				if (currVertIndex.posIndex >= 0) {
					currVert.position = basePositions[currVertIndex.posIndex];
				}
				if (currVertIndex.normalIndex >= 0) {
					currVert.normal = baseNormals[currVertIndex.normalIndex];
				}
				else {
					currVert.normal = newNorm;
				}
				if (currVertIndex.texCoordIndex > 0) {
					currVert.texCoords = baseTexCoords[currVertIndex.texCoordIndex];
				}
				// with everything set, push back to our VAO and VBO vectors
				VAOVertices.push_back(currVert);
				VBOIndices.push_back(i * 3 + j);
			}
		}
	}
}

void Object::CalculateCenter() {
	// set bounds
	glm::vec3 maxBound(INT_MIN);
	glm::vec3 minBound(INT_MAX);
	for (auto vertex : VAOVertices) {
		maxBound[0] = std::max(vertex.position[0], maxBound[0]);
		maxBound[1] = std::max(vertex.position[1], maxBound[1]);
		maxBound[2] = std::max(vertex.position[2], maxBound[2]);
		minBound[0] = std::min(vertex.position[0], minBound[0]);
		minBound[1] = std::min(vertex.position[1], minBound[1]);
		minBound[2] = std::min(vertex.position[2], minBound[2]);
	}
	// finally, calculate object center point
	objCenter = (maxBound + minBound) * 0.5f;
}