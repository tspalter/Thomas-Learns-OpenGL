#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "Animation.h"

class Animator {
	public:
		// ctor that chooses animation to play
		Animator(Animation* animation);

		// advances time and calls CalculateBoneTransform
		void UpdateAnimation(float dt);
		// init function for animation starting at time 0
		void PlayAnimation(Animation* pAnimation);
		// checks for a bone based on the node name, then interpolates if found
		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		// used to draw in bone locations for debugging purposes
		void DebugDraw();
		// private variable get function
		std::vector<glm::mat4> GetFinalBoneMatrices();

		std::vector<float> boneVertices;
		std::vector<GLuint> boneIndices;
		GLuint VAO, VBO, EBO;

		GLuint indexNumber = 0;

	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		Animation* m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime;
};

#endif