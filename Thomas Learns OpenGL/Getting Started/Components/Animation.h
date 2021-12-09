#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <map>
#include <glm/glm/glm.hpp>
#include <assimp/scene.h>
#include <functional>

#include "Bone.h"
#include "AnimData.h"
#include "Model.h"

// struct that collects data from ASSIMP nodes
struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation {
public:
	// default ctor
	Animation() = default;
	// builds animation from a provided file and model
	Animation(const std::string& animationPath, Model* model);
	~Animation() { }

	// locates bone based on its name
	Bone* FindBone(const std::string& name);

	// get functions for private vars
	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

private:
	// useful for loading models and animations separately, as sometimes bones may be missing
	void ReadMissingBones(const aiAnimation* animation, Model& model);
	// replicate aiNode hierarchy in ASSIMP to create a new hierarchy of AssimpNodeData
	void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};

#endif // !ANIMATION_H

