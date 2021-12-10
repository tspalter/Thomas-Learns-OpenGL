// Light.h
// header file that handles different light structs and a class for lighting
#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

// object class that handles different types of light
class Light {
	// struct for directional light
	struct DirectionLight {
		bool status;
		glm::vec3 direction;

		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	// struct for point light
	struct PointLight {
		bool status;
		glm::vec3 position;
		float constant;
		float linear;
		float quadratic;

		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

public:
	// lighting system contains both structs, they can be
	// activated/deactivated through their status variable
	DirectionLight dLight;
	PointLight pLight;

	// ctor
	Light();
	// dtor
	~Light();

	// initialize function, currently values are
	// hard coded to be used by the shaders
	void InitLight();
};

#endif