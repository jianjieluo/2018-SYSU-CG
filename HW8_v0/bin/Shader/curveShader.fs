#version 330 core

uniform  vec3 curveColor;
out vec4 FragColor;

void main()
{
    // FragColor = vec4(0.2f, 0.6f, 0.8f, 1.0f);
	FragColor = vec4(curveColor, 1.0f);
} 