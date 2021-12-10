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
		{ 0.5f, 4.0f, 0.5f }, // yellow bone
		{ 0.5f, 3.0f, 0.5f }, // purple bone
		{ 0.5f, 2.0f, 0.5f }  // blue bone
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
		// set the Jacobian matrix
		SetJacobian(0);

		// set up the angles vector
		anglesVector = GetTranspose(0) * goalVector;
		anglesVector = anglesVector * alpha;

		// update the angle values from the new anglesVector
		angles[0] += anglesVector(0, 0); // Yellow Z
		angles[1] += anglesVector(1, 0); // Purple Z
		angles[2] += anglesVector(2, 0); // Blue Z
		angles[3] += anglesVector(3, 0); // Yellow Y
		angles[4] += anglesVector(4, 0); // Purple Y
		angles[5] += anglesVector(5, 0); // Blue Y
		angles[6] += anglesVector(6, 0); // Yellow X
		angles[7] += anglesVector(7, 0); // Purple X
		angles[8] += anglesVector(8, 0); // Blue X

		InverseKinematics(angles[6], angles[3], angles[0],
			angles[7], angles[4], angles[1],
			angles[8], angles[5], angles[2]);

		// check if we should stop to avoid unnecessary calculation
		ShouldStop();
	}
}

void IKBoneAnimation::ShouldStop() {
	// check if the distance is within the error factor
	if ((glm::length(target - bluePositionEnd) < errorFactor)) {
		// stop the movement
		stop = true;
		didBoneMove = false;
		prevDist = 100;
	}
	else {
		prevDist = glm::length(target - bluePositionEnd);
	}
}

void IKBoneAnimation::SetJacobian(int id) {
	// difference between target position and end of our blue piece
	goal = target - bluePositionEnd;
	goalVector << goal.x, goal.y, goal.z;

	// update vector values
	v1 = glm::cross(glm::normalize(glm::vec3(mYellowMat[2])), (bluePositionEnd - redPositionEnd)); // Yellow Z
	v2 = glm::cross(glm::normalize(glm::vec3(mPurpleMat[2])), (bluePositionEnd - yellowPositionEnd)); // Purple Z
	v3 = glm::cross(glm::normalize(glm::vec3(mBlueMat[2])), (bluePositionEnd - purplePositionEnd)); // Blue Z

	v4 = glm::cross(glm::normalize(glm::vec3(mYellowMat[1])), (bluePositionEnd - redPositionEnd)); // Yellow Y
	v5 = glm::cross(glm::normalize(glm::vec3(mPurpleMat[1])), (bluePositionEnd - yellowPositionEnd)); // Purple Y
	v6 = glm::cross(glm::normalize(glm::vec3(mBlueMat[1])), (bluePositionEnd - purplePositionEnd)); // Blue Y

	v7 = glm::cross(glm::normalize(glm::vec3(mYellowMat[0])), (bluePositionEnd - redPositionEnd)); // Yellow X
	v8 = glm::cross(glm::normalize(glm::vec3(mPurpleMat[0])), (bluePositionEnd - yellowPositionEnd)); // Purple X
	v9 = glm::cross(glm::normalize(glm::vec3(mBlueMat[0])), (bluePositionEnd - purplePositionEnd)); // Blue X

	// update the Jacobian matrix
	jacobian <<
		v1.x, v2.x, v3.x, v4.x, v5.x, v6.x, v7.x, v8.x, v9.x,
		v1.y, v2.y, v3.y, v4.y, v5.y, v6.y, v7.y, v8.y, v9.y,
		v1.z, v2.z, v3.z, v4.z, v5.z, v6.z, v7.z, v8.z, v9.z;
}

Eigen::MatrixXf IKBoneAnimation::GetTranspose(int id) {
	return (jacobian.transpose());
}

void IKBoneAnimation::InverseKinematics(float angleYellowX, float angleYellowY, float angleYellowZ,
	float anglePurpleX, float anglePurpleY, float anglePurpleZ,
	float angleBlueX, float angleBlueY, float angleBlueZ) {

	// initialize scale
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	//-------------------------------------------------------
	// YELLOW MATRIX
	mYellowMat = glm::mat4(1.0f);

	// handle rotations and store in rot
	// Z
	mYellowMat = glm::rotate(mYellowMat, glm::radians(angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	rot[0] = (glm::normalize(mYellowMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	// Y
	mYellowMat = glm::rotate(mYellowMat, glm::radians(angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	rot[1] = (glm::normalize(mYellowMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
	// X
	mYellowMat = glm::rotate(mYellowMat, glm::radians(angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));
	rot[2] = (glm::normalize(mYellowMat * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));

	// set matrix back to original values for now
	mYellowMat = glm::rotate(mYellowMat, glm::radians(-angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mYellowMat = glm::rotate(mYellowMat, glm::radians(-angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mYellowMat = glm::rotate(mYellowMat, glm::radians(-angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));

	// reset matrix
	mYellowMat = glm::mat4(1.0f);

	// scale
	glm::scale(mYellowMat, scale);

	// important vars
	glm::vec3 pivot = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec3 pos = rootPosition;

	// make final transforms
	mYellowMat = glm::translate(mYellowMat, pos);
	mYellowMat = glm::rotate(mYellowMat, glm::radians(angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mYellowMat = glm::rotate(mYellowMat, glm::radians(angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mYellowMat = glm::rotate(mYellowMat, glm::radians(angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));
	mYellowMat = glm::translate(mYellowMat, pivot);

	mYellowMat = glm::translate(mYellowMat, glm::vec3(0.0f, 2.0f, 0.0f));
	yellowPositionEnd = glm::vec3(mYellowMat[3]);
	mYellowMat = glm::translate(mYellowMat, glm::vec3(0.0f, -2.0f, 0.0f));
	mYellowMat = glm::scale(mYellowMat, scaleVector[1]);

	//-------------------------------------------------------
	// PURPLE MATRIX
	mPurpleMat = glm::mat4(1.0f);

	// handle initial rotations of the previous bone(s)
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));

	// handle rotations and store in rot
	// Z
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(anglePurpleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	rot[3] = (glm::normalize(mPurpleMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	// Y
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(anglePurpleY), glm::vec3(0.0f, 1.0f, 0.0f));
	rot[4] = (glm::normalize(mPurpleMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
	// X
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(anglePurpleX), glm::vec3(1.0f, 0.0f, 0.0f));
	rot[5] = (glm::normalize(mPurpleMat * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));

	// set matrix back to original values for now
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(-angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(-angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(-angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));

	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(-anglePurpleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(-anglePurpleY), glm::vec3(0.0f, 1.0f, 0.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(-anglePurpleX), glm::vec3(1.0f, 0.0f, 0.0f));

	// reset matrix
	mPurpleMat = glm::mat4(1.0f);

	// scale
	glm::scale(mPurpleMat, scale);

	// important vars
	glm::vec3 pivot2 = glm::vec3(0.0f, 5.5f, 0.0f);
	glm::vec3 pos2 = rootPosition;

	// make final transforms
	mPurpleMat = glm::translate(mPurpleMat, pos2);
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));
	mPurpleMat = glm::translate(mPurpleMat, pivot2);

	glm::vec3 pp = glm::vec3(0.0f, -1.5f, 0.0f);

	mPurpleMat = glm::translate(mPurpleMat, pp);
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(anglePurpleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(anglePurpleY), glm::vec3(0.0f, 1.0f, 0.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(anglePurpleX), glm::vec3(1.0f, 0.0f, 0.0f));
	mPurpleMat = glm::translate(mPurpleMat, -pp);

	mPurpleMat = glm::translate(mPurpleMat, glm::vec3(0.0f, 1.5f, 0.0f));
	purplePositionEnd = glm::vec3(mPurpleMat[3]);
	mPurpleMat = glm::translate(mPurpleMat, glm::vec3(0.0f, -1.5f, 0.0f));
	mPurpleMat = glm::scale(mPurpleMat, scaleVector[2]);

	//-------------------------------------------------------
	// BLUE MATRIX
	mBlueMat = glm::mat4(1.0f);

	// handle initial rotations of the previous bone(s)
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));

	mBlueMat = glm::rotate(mBlueMat, glm::radians(anglePurpleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(anglePurpleY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(anglePurpleX), glm::vec3(1.0f, 0.0f, 0.0f));

	// handle rotations and store in rot
	// Z
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleBlueZ), glm::vec3(0.0f, 0.0f, 1.0f));
	rot[6] = (glm::normalize(mPurpleMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	// Y
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleBlueY), glm::vec3(0.0f, 1.0f, 0.0f));
	rot[7] = (glm::normalize(mPurpleMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
	// X
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleBlueX), glm::vec3(1.0f, 0.0f, 0.0f));
	rot[8] = (glm::normalize(mPurpleMat * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));

	// set matrix back to original values for now
	mBlueMat = glm::rotate(mPurpleMat, glm::radians(-angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mPurpleMat, glm::radians(-angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mPurpleMat, glm::radians(-angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));

	mBlueMat = glm::rotate(mBlueMat, glm::radians(-anglePurpleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(-anglePurpleY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(-anglePurpleX), glm::vec3(1.0f, 0.0f, 0.0f));

	mBlueMat = glm::rotate(mBlueMat, glm::radians(-angleBlueZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(-angleBlueY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(-angleBlueX), glm::vec3(1.0f, 0.0f, 0.0f));

	// reset matrix
	mBlueMat = glm::mat4(1.0f);

	// scale
	glm::scale(mBlueMat, scale);

	// important vars
	glm::vec3 pivot3 = glm::vec3(0.0f, 8.0f, 0.0f);
	glm::vec3 pos3 = rootPosition;

	// make final transforms
	mBlueMat = glm::translate(mBlueMat, pos3);
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleYellowZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleYellowY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleYellowX), glm::vec3(1.0f, 0.0f, 0.0f));
	mBlueMat = glm::translate(mBlueMat, pivot3);

	glm::vec3 pp2 = glm::vec3(0.0f, -4.0f, 0.0f);

	mBlueMat = glm::translate(mBlueMat, pp2);
	mBlueMat = glm::rotate(mBlueMat, glm::radians(anglePurpleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(anglePurpleY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(anglePurpleX), glm::vec3(1.0f, 0.0f, 0.0f));
	mBlueMat = glm::translate(mBlueMat, -pp2);

	glm::vec3 pp3 = glm::vec3(0.0f, -1.0f, 0.0f);

	mBlueMat = glm::translate(mBlueMat, pp3);
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleBlueZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleBlueY), glm::vec3(0.0f, 1.0f, 0.0f));
	mBlueMat = glm::rotate(mBlueMat, glm::radians(angleBlueX), glm::vec3(1.0f, 0.0f, 0.0f));
	mBlueMat = glm::translate(mBlueMat, -pp3);

	mBlueMat = glm::translate(mBlueMat, glm::vec3(0.0f, 1.0f, 0.0f));
	bluePositionEnd = glm::vec3(mBlueMat[3]);
	mBlueMat = glm::translate(mBlueMat, glm::vec3(0.0f, -1.0f, 0.0f));
	mBlueMat = glm::scale(mBlueMat, scaleVector[3]);
}

void IKBoneAnimation::RotatePurple(float angleX, float angleY, float angleZ) {
	// initialize purple matrix
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	mPurpleMat = glm::mat4(1.0f);
	mPurpleMat = glm::scale(mPurpleMat, scale);

	glm::vec3 pivot = glm::vec3(0.0f, 30.0f, 0.0f);
	glm::vec3 pos = yellowPosition;

	// make transformations
	mPurpleMat = glm::translate(mPurpleMat, pos);
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	mPurpleMat = glm::rotate(mPurpleMat, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	mPurpleMat = glm::translate(mPurpleMat, pivot);

	mPurpleMat = glm::scale(mPurpleMat, scaleVector[2]);
}

void IKBoneAnimation::Reset() {
	// reset yellow angles
	yellowXangle = 0.0f;
	yellowYangle = 0.0f;
	yellowZangle = 0.0f;

	// reset the matrices
	// yellow
	mYellowMat = glm::mat4(1.0f);
	mYellowMat = glm::translate(mYellowMat, yellowPosition);
	mYellowMat = glm::scale(mYellowMat, scaleVector[1]);
	// purple
	mPurpleMat = glm::mat4(1.0f);
	mPurpleMat = glm::translate(mPurpleMat, purplePosition);
	mPurpleMat = glm::scale(mPurpleMat, scaleVector[2]);
	// blue
	mBlueMat = glm::mat4(1.0f);
	mBlueMat = glm::translate(mBlueMat, bluePosition);
	mBlueMat = glm::scale(mBlueMat, scaleVector[3]);

	// reset other variables
	stop = false;
	positionChanged = false;
	time = 0.0f;
	errorFactor = 0.1f;

	target = glm::vec3(3.0f, 8.0f, 3.0f);
	redPositionEnd = rootPosition;
	alpha = 0.1f;

	angles[0] = 0.0f;
	angles[1] = 0.0f;
	angles[2] = 0.0f;
	angles[3] = 0.0f;
	angles[4] = 0.0f;
	angles[5] = 0.0f;

	mTargetMat = glm::mat4(1.0f);
	mTargetMat = glm::translate(mTargetMat, target);
}