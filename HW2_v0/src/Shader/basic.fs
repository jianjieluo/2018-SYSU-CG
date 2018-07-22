#version 330 core

out vec4 FragColor;
in vec4 vColor;

uniform bool isdefault;
uniform vec4 uni_color;

void main() {
	FragColor = isdefault ? vColor : uni_color;
}