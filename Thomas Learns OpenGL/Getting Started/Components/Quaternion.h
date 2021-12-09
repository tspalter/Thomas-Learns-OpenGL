#ifndef QUATERNION_H
#define QUATERNION_H

#include <glad/glad.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

// quaternion class
class Quaternion {
	public:
		// variables for the quaternion
		float S;
		glm::vec3 V;

		// ctors
		Quaternion() {
			S = 1.0f;
			V = glm::vec3(0.0f);
		}

		Quaternion(float w, float x, float y, float z) {
			S = w;
			V = glm::vec3(x, y, z);
		}

		// unnecessary for runtime, mainly used for debugging purposes
		/*Quaternion(glm::quat q) {
			S = q.w;
			V = glm::vec3(q.x, q.y, q.z);
		}*/

		// returns the magnitude of a quaternion
		float GetMagnitude() {
			return sqrt(pow(S, 2) + pow(V.x, 2) + pow(V.y, 2) + pow(V.z, 2));
		}

		// operator overloads
		Quaternion operator+(const Quaternion& q) const {
			Quaternion result;
			result.S = this->S + q.S;
			result.V = this->V + q.V;

			return result;
		}

		Quaternion operator*(const Quaternion& q) const {
			Quaternion result;
			result.S = (this->S * q.S) - glm::dot(this->V, q.V);
			result.V = (this->S * q.V) + (q.S * this->V) + (this->V * q.V);

			return result;
		}

		Quaternion operator*(const float a) const {
			return Quaternion(a * S, a * V.x, a * V.y, a * V.z);
		}

	private:
		

};

// recreation of glm's mix function to implement Slerp
inline Quaternion mix(Quaternion q1, Quaternion q2, float a) {
	return q1 * (1.0f - a) + q2 * a;
}

// Implement Slerp
inline Quaternion Slerp(Quaternion q1, Quaternion q2, float scaleFactor) {
	return mix(q1, q2, scaleFactor);
}

// normalize the quaternion
inline Quaternion normalize(Quaternion q) {
	float d = q.GetMagnitude();
	return Quaternion(q.S / d, q.V.x / d, q.V.y / d, q.V.z / d);
}

// turn quaternion into a 4x4 matrix
inline glm::mat4 QuatToMat4(Quaternion q) {
	glm::mat4 Result(1.0f);

	float qxx = q.V.x * q.V.x;
	float qyy = q.V.y * q.V.y;
	float qzz = q.V.z * q.V.z;
	float qxz = q.V.x * q.V.z;
	float qxy = q.V.x * q.V.y;
	float qyz = q.V.y * q.V.z;
	float qwx = q.S * q.V.x;
	float qwy = q.S * q.V.y;
	float qwz = q.S * q.V.z;

	Result[0][0] = 1.0f - 2.0f * (qyy + qzz);
	Result[0][1] = 2.0f * (qxy + qwz);
	Result[0][2] = 2.0f * (qxz - qwy);

	Result[1][0] = 2.0f * (qxy - qwz);
	Result[1][1] = 1.0f - 2.0f * (qxx + qzz);
	Result[1][2] = 2.0f * (qyz + qwx);

	Result[2][0] = 2.0f * (qxz + qwy);
	Result[2][1] = 2.0f * (qyz - qwx);
	Result[2][2] = 1.0f - 2.0f * (qxx + qyy);
	return Result;
}

// dot product function for Quaternion class
inline float dot(Quaternion q1, Quaternion q2) {
	return (q1.S * q2.S) + glm::dot(q1.V, q2.V);
}

// returns inverse of the provided quaternion
inline Quaternion inverse(Quaternion q) {
	Quaternion result;
	result.V = -q.V;
	float magnitude = q.GetMagnitude();
	return result * (1.0f / pow(magnitude, 2));
}

#endif