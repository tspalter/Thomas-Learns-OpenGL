// IKBoneAnimation.h
// header file for the IKBoneAnimation object class
// handles the math and functions for animating the bones
#ifndef IKBONEANIMATION_H
#define IKBONEANIMATION_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <math.h>
#include "../../Eigen/Dense"
#include <list>

class IKBoneAnimation {
public:
	// ctor
	IKBoneAnimation();
	// dtor
	~IKBoneAnimation();

	void Init();
	void Update(float dT);
	void InverseKinematics(float angleYellowX, float angleYellowY, float angleYellowZ,
		float anglePurpleX, float anglePurpleY, float anglePurpleZ,
		float angleBlueX, float angleBlueY, float angleBlueZ);
	void RotatePurple(float angleX, float angleY, float angleZ);

	glm::tvec4<float>* rot = new glm::tvec4<float>[9];

	// new matrices from the Eigen library
	Eigen::MatrixXf jacobian = Eigen::MatrixXf::Zero(3, 9); // Jacobian matrix
	Eigen::MatrixXf anglesVector = Eigen::MatrixXf::Zero(9, 1); // for degrees of freedom
	Eigen::MatrixXf goalVector = Eigen::MatrixXf::Zero(3, 1); // for the joints
	bool didBoneMove;
	float prevDist = 100;

	// get the Jacobian matrix's transpose
	Eigen::MatrixXf GetTranspose(int id);

	void Reset();
	void ShouldStop();
	void ComputeAngles();
	void SetJacobian(int id);

	// Get functions
	glm::mat4 GetYellowMat() { return mYellowMat; };
	glm::mat4 GetPurpleMat() { return mPurpleMat; };
	glm::mat4 GetBlueMat() { return mBlueMat; };
	glm::mat4 GetTargetMat() { return mTargetMat; };

public:
	// additional public vars

	// Here the head of each vector is the root bone
	std::vector<glm::vec3> scaleVector;
	std::vector<glm::vec3> rotDegreeVector;
	std::vector<glm::vec4> colors;

	// position values
	glm::vec3 rootPosition;
	glm::vec3 yellowPosition;
	glm::vec3 purplePosition;
	glm::vec3 bluePosition;

	// yellow angles
	float yellowXangle;
	float yellowYangle;
	float yellowZangle = 30; 

	// purple angles
	float purpleXangle;
	float purpleYangle;
	float purpleZangle = 30;

	// blue angles
	float blueXangle;
	float blueYangle;
	float blueZangle = 30;

	// rotation vectors
	glm::vec3 yellowRot;
	glm::vec3 purpleRot;
	glm::vec3 blueRot;

	// angle vector vars
	glm::vec3 lastTargetPosition, goal, v1, v2, v3, v4, v5, v6, v7, v8, v9, anglesVectorX, anglesVectorY, anglesVectorZ;

	// alpha values
	float alphaYellow, alphaPurple, alphaBlue;
	// additional vars
	float angles[9];
	float time, errorFactor, alpha, limitS0, limitS1, limitS2;

	// bone endpoints
	glm::vec3 yellowPositionEnd, purplePositionEnd, bluePositionEnd, target, redPositionEnd;

	bool stop, positionChanged;

private:
	glm::mat4 mYellowMat;
	glm::mat4 mPurpleMat;
	glm::mat4 mBlueMat;
	glm::mat4 mTargetMat;

};

#endif