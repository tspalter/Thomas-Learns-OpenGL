// Light.cpp
// source file for the Light object class
// designed to keep all lighting values organized in one source file

#include "Light.h"

Light::Light() { }

Light::~Light() { }

void Light::InitLight() {
	// set directional light values
	dLight.status = true;
	dLight.direction = glm::vec3(-1.0f, -1.0f, -1.0f);
	dLight.ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	dLight.diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	dLight.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// set point light values
	pLight.status = true;
	pLight.direction = glm::vec3(1.2f, 1.0f, 2.0f);
	pLight.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	pLight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pLight.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pLight.constant = 1.0f;
	pLight.linear = 0.09f;
	pLight.quadratic = 0.032f;
}