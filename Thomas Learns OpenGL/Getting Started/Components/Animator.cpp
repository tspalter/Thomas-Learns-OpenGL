#include "Animator.h"

Animator::Animator(Animation* animation) {
	m_CurrentTime = 0.0f;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++) {
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::UpdateAnimation(float dt) {
	m_DeltaTime = dt;
	boneVertices.clear();
	if (m_CurrentAnimation) {
		// update time and calculate any potential bone transformations
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation) {
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	// find the bone
	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);
	// does bone exist?  Then interpolate
	if (Bone) {
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}
	// find the global transformation
	glm::mat4 globalTransformation = parentTransform * nodeTransform;
	// push back values for debug drawing
	glm::vec3 boneDebugDraw = globalTransformation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	boneVertices.push_back(boneDebugDraw.x);
	boneVertices.push_back(boneDebugDraw.y);
	boneVertices.push_back(boneDebugDraw.z);

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}
	// recursively call based on node children
	GLuint indSize = indexNumber;
	for (int i = 0; i < node->childrenCount; i++) {
		boneIndices.push_back(indSize);
		boneIndices.push_back(++indexNumber);
		CalculateBoneTransform(&node->children[i], globalTransformation);
	}
}

void Animator::DebugDraw() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind VAO
	glBindVertexArray(VAO);
	// copy our vertices array in vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, boneVertices.size() * sizeof(float), &boneVertices[0], GL_STATIC_DRAW);
	// copy index array in element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, boneIndices.size() * sizeof(GLuint), &boneIndices[0], GL_STATIC_DRAW);
	// set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_LINES, boneIndices.size(), GL_UNSIGNED_INT, 0);
	
	glPointSize(10);
	glDrawArrays(GL_POINTS, boneVertices[0], boneVertices.size());
	
	glBindVertexArray(0);
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices() {
	return m_FinalBoneMatrices;
}