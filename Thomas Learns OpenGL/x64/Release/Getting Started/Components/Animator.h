#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "Animation.h"

class Animator {
	public:
		Animator(Animation* animation);

		void UpdateAnimation(float dt);
		void PlayAnimation(Animation* pAnimation);
		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		void DebugDraw();
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