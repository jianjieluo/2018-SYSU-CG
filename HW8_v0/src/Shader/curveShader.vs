#version 330 core
layout (location = 0) in float t;

uniform vec3 p0;
uniform vec3 p1;
uniform vec3 p2;
uniform vec3 p3;


void main()
{
	vec3 qt = pow((1-t), 3) * p0 + 3 * t * (1-t) * (1-t) * p1 + 3 * t * t * (1-t) * p2 + t * t * t * p3;
    gl_Position = vec4(qt.x, qt.y, qt.z, 1.0);
}