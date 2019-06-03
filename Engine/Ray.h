#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <random>

class Ray
{
public:
	Ray() {}
	Ray(const glm::vec3& a, const glm::vec3& b, float ti = 0.0 ) 
	{ 
		A = a; 
		B = b;
		_time = ti; 
		invdir =glm::vec3( 1.0f / B.x, 1.0f / B.y, 1.0f / B.z);
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);

	}
	glm::vec3 origin() const { return A; }
	glm::vec3 direction() const { return B; }
	float time() const { return _time; }
	glm::vec3 point_at_parameter(float t) const { return A + t * B; }
	~Ray() {}

	glm::vec3 A;
	glm::vec3 B;
	int sign[3];
	glm::vec3 invdir;
	float  _time;
};

