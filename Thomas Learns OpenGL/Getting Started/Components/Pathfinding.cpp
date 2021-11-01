#include "Pathfinding.h"

Point3D GetSpline(std::vector<Point3D> points, float t) {
	// initialize control points used
	int p0, p1, p2, p3;
	// mod operator allows spline to use initial points a second time in order to form the continuous loop
	p1 = (int)t % points.size();
	p2 = (p1 + 1) % points.size();
	p3 = (p2 + 1) % points.size();
	p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;

	// alter t-value and create vars for t^2 and t^3
	t = t - (int)t;
	float tSquared = t * t;
	float tCubed = tSquared * t;

	// calculate q values
	float q1 = -tCubed + 2.0f * tSquared - t;
	float q2 = 3.0f * tCubed - 5.0f * tSquared + 2.0f;
	float q3 = -3.0f * tCubed + 4.0f * tSquared + t;
	float q4 = tCubed - tSquared;

	// finally, evaluate transformed values for spline point at value t
	float tX = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
	float tY = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);
	float tZ = 0.5f * (points[p0].z * q1 + points[p1].z * q2 + points[p2].z * q3 + points[p3].z * q4);

	return { tX, tY, tZ };
}

std::vector<float> GetArcLengths(std::vector<Point3D> splinePoints) {
	std::vector<float> arcLengthList;
	
	// iterate through the spline points to determine the different arc lengths
	for (int i = 0; i < splinePoints.size(); i++) {
		// value of the arc length at i
		float arcLength = 0.0f;
		glm::vec3 point = glm::vec3(splinePoints[i].x, splinePoints[i].y, splinePoints[i].z);
		point = glm::normalize(point);
		if (i > 0) {
			// find the previous point
			glm::vec3 prevPoint = glm::vec3(splinePoints[i - 1].x, splinePoints[i - 1].y, splinePoints[i - 1].z);
			prevPoint = glm::normalize(prevPoint);
			arcLength = arcLengthList[i - 1] + GetDistance(prevPoint, point);
		}
		arcLengthList.push_back(arcLength);
	}

	return arcLengthList;
}

tArcValues GetFinalPairValues(std::vector<float> arcLengths, float curveLength) {
	tArcValues tArcPairs;
	for (int i = 0; i < arcLengths.size(); i++) {
		std::pair<float, float> tArc;
		tArc.first = arcLengths[i] / curveLength;
		tArc.second = arcLengths[i];
		tArcPairs.push_back(tArc);
	}
	return tArcPairs;
}

float FindArcLength(tArcValues arcValuePairs, int l, int r, float t) {
	if (r >= 1) {
		int mid = l + (r - 1) / 2;

		// at midpoint
		if (arcValuePairs[mid].first == t) {
			return arcValuePairs[mid].second;
		}

		// if element is smaller than mid t
		if (arcValuePairs[mid].first < t) {
			return FindArcLength(arcValuePairs, l, mid - 1, t);
		}

		// element is larger than mid t
		if (arcValuePairs[mid].first > t) {
			return FindArcLength(arcValuePairs, mid + 1, r, t);
		}
	}

	// element is not present
	return -1.0f;
}

float GetDistance(glm::vec3 p1, glm::vec3 p2) {
	return glm::sqrt(glm::pow(p2.x - p1.x, 2) + glm::pow(p2.y - p1.y, 2) + glm::pow(p2.z - p1.z, 2));
}

std::vector<float> ConvertPointsToFloats(std::vector<Point3D> points) {
	std::vector<float> pointCoords;
	for (int i = 0; i < points.size(); i++) {
		pointCoords.push_back(points[i].x);
		pointCoords.push_back(points[i].y);
		pointCoords.push_back(points[i].z);
	}

	return pointCoords;
}

void DrawPath(Shader& shader, std::vector<float> pointCoords) {
	shader.use();
	std::vector<int> inds =
	{
		0, 1,
		1, 2, 
		2, 3,
		3, 4,
		4, 5,
		5, 6,
		6, 7,
		7, 0
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind VAO
	glBindVertexArray(VAO);
	// copy our vertices array in vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pointCoords.size() * sizeof(float), &pointCoords[0], GL_STATIC_DRAW);
	
	// copy index array in element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(GLuint), &inds[0], GL_STATIC_DRAW);
	// set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_LINES, inds.size(), GL_UNSIGNED_INT, 0);

	// for some reason, all of the points aren't being drawn in.  Therefore I'm only using the polygonal shape
	// created by the control points to draw in their location
	/*glPointSize(10);
	glDrawArrays(GL_POINTS, pointCoords[0], pointCoords.size());*/
	glBindVertexArray(0);
}

void DrawSplinePath(Shader& shader, std::vector<float> pointCoords) {
	shader.use();
	std::vector<int> inds;
	for (int i = 0; i < 500; i++) {
		if (i == 499) {
			inds.push_back(i);
			inds.push_back(0);
		}
		else {
			inds.push_back(i);
			inds.push_back(i + 1);
		}
	}

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind VAO
	glBindVertexArray(VAO);
	// copy our vertices array in vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pointCoords.size() * sizeof(float), &pointCoords[0], GL_STATIC_DRAW);

	// copy index array in element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(GLuint), &inds[0], GL_STATIC_DRAW);
	// set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_LINES, inds.size(), GL_UNSIGNED_INT, 0);

	glPointSize(2);
	glDrawArrays(GL_POINTS, pointCoords[0], pointCoords.size());
	glBindVertexArray(0);
}