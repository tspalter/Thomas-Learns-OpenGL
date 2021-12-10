// Engine.h
// header file for Engine object class
// handles all engine-related functions
// learned from working with a game engine on my team

#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/string_cast.hpp>


#include "Shader.h"
#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "IKBoneAnimation.h"

class Engine {
public:
	// variables
	GLFWwindow* mWindow;
	static Camera* mCamera;
	static Light* mLighting;
	static IKBoneAnimation* mBoneAnim;
	std::vector<Object> objList;
	
	glm::vec4 backgroundColor = glm::vec4(0.0f, 0.2f, 0.2f, 1.0f);
	bool isSceneReset = true;
	std::string modelName;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	static bool keys[1024];

public:
	// functions

	// ctor
	Engine();
	// dtor
	~Engine();

	// initial engine functions
	void Init();

	// main display function, handles update loop
	void Display(GLFWwindow* window);
	// run the program, handles initialization and update
	void Run();

	// handles camera movement
	void OnCameraMove();
	// loads .obj models
	void LoadModels();

	// draws the scene
	void DrawScene(Shader& shader);
	// draw a specific object
	void DrawObject(Shader& shader, Object& object);

	// additional draw functions based on which object is being drawn
	void DrawAxis(Shader& shader, const glm::mat4 axisObjMat);
	void DrawPlane(Shader& shader);
	void DrawBones(Shader& shader, IKBoneAnimation* mBoneAnimation);

	// additional functions
	void BindVAOVBO(Object& currObj);
	void SetupUniformValues(Shader& shader);
	void SceneReset();

};

#endif
