#include "IKBoneAnimation.h"

IKBoneAnimation::IKBoneAnimation() { }

IKBoneAnimation::~IKBoneAnimation() { }

void IKBoneAnimation::Init() {
	// initialize positions
	rootPosition = { 2.0f, 1.0f, 2.0f };
	yellowPosition = { 2.0f, 3.0f, 2.0f };
	purplePosition = { 2.0f, 6.5f, 2.0f };
	bluePosition = { 2.0f, 9.0f, 2.0f };

	// initialize the vectors
	// scale
	scaleVector = {
		{ 1.0f, 1.0f, 1.0f }, // root
		{ 0.5f, 3.0f, 0.5f }, // yellow bone
		{ 0.5f, 3.0f, 0.5f }, // purple bone
		{ 0.5f, 3.0f, 0.5f }  // blue bone
	};
	// rotation degree
	rotDegreeVector = {
		{ 0.0f, 0.0f, 0.0f }, // root
		{ 0.0f, 0.0f, 0.0f }, // yellow
		{ 0.0f, 0.0f, 0.0f }, // purple
		{ 0.0f, 0.0f, 0.0f }  // blue
	};
	// colors
	colors = {
		{ 0.7f, 0.0f, 0.0f, 1.0f }, // red for root
		{ 0.7f, 0.7f, 0.0f, 1.0f }, // yellow
		{ 0.7f, 0.0f, 0.7f, 1.0f }, // purple
		{ 0.0f, 0.7f, 0.7f, 1.0f }, // blue
		{ 0.2f, 0.6f, 0.3f, 1.0f }  // green for target position
	};

	// call Reset to make sure everything is set in a default state
	Reset();
}

void IKBoneAnimation::Update(float dT) {
	// set up target matrix
	mTargetMat = glm::mat4(1.0f);
	// translate
	mTargetMat = glm::translate(mTargetMat, target);

	// create test vector
	glm::vec3 test = target - redPositionEnd;

	// check for movement
	if (didBoneMove) {
		stop = false;
		// make sure angles are updated
		ComputeAngles();
	}
}

void IKBoneAnimation::ComputeAngles() {
	// make sure that we're in motion and the angles should be re-calculated
	if (!stop) {
		// reset the Jacobian matrix
		SetJacobian(0);

		// set up the angles vector
		anglesVector = GetTranspose(0) * goalVector;
		anglesVector = anglesVector * alpha;

		// update the angle values from the new anglesVector
		angles[0] += anglesVector(0, 0);
		angles[1] += anglesVector(1, 0);
		angles[2] += anglesVector(2, 0);
		angles[3] += anglesVector(3, 0);
		angles[4] += anglesVector(4, 0);
		angles[5] += anglesVector(5, 0);
		angles[6] += anglesVector(6, 0);
		angles[7] += anglesVector(7, 0);
		angles[8] += anglesVector(8, 0);

	}
}