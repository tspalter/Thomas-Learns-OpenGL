#version 430 core

uniform bool pathColor;
uniform bool controlColor;

out vec4 FragColor;

void main() {
	if (pathColor) {
		FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (controlColor) {
		FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else {
		FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
}