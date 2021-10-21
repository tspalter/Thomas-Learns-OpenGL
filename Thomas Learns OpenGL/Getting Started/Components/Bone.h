#pragma once

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/quaternion.hpp>

#include "AssimpGLMHelpers.h"

// struct for position of key frame
struct KeyPosition {
	glm::vec3 position;
	float timeStamp;
};

// struct for rotation of key frame
struct KeyRotation {
	Quaternion orientation;
	float timeStamp;
};

// struct for scale of key frame
struct KeyScale {
	glm::vec3 scale;
	float timeStamp;
};

class Bone {
	public:
		// reads in key frames from aiNodeAnim (ASSIMP Node Animation)
		Bone(const std::string& name, int ID, const aiNodeAnim* channel);
		// interpolates between positions, rotations, and scaling keys based on the
		// current time of the animation, prepares local transformation matrix by combining
		// key transforms
		void Update(float animationTime);
		// functions to get private variables
		glm::mat4 GetLocalTransform();
		std::string GetBoneName() const;
		int GetBoneID();

		// gets current index on mKeyPositions in order to interpolate based on
		// current animation time
		int GetPositionIndex(float animationTime);
		// gets current index on mKeyRotations in order to interpolate based on
		// current animation time
		int GetRotationIndex(float animationTime);
		// gets current index on mKeyScalings in order to interpolate based on
		// current animation time
		int GetScaleIndex(float animationTime);

	private:
		// normalized value for lerp/slerp
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
		// figures out which position keys to interpolate between, perform interpolation, 
		// and return the transformation matrix
		glm::mat4 InterpolatePosition(float animationTime);
		// figures out which rotation keys to interpolate between, perform interpolation, 
		// and return the rotation matrix
		glm::mat4 InterpolateRotation(float animationTime);
		// figures out which scaling keys to interpolate between, perform interpolation, 
		// and return the scale matrix
		glm::mat4 InterpolateScaling(float animationTime);

		// private variables
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;
		int m_NumPositions;
		int m_NumRotations;
		int m_NumScalings;

		glm::mat4 m_LocalTransform;
		std::string m_Name;
		int m_ID;
};