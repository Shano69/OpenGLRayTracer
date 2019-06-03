#pragma once

#include <glm/glm.hpp>
#include <random>

class Perlin
{
public:
	float noise(const glm::vec3& p) const;
	static float* perlin_generate();
	static int* perlin_generate_perm();
	float turb(const glm::vec3& p, int depth = 7) const;


	static float *ranfloat;
	static int *perm_x;
	static int *perm_y;
	static int *perm_z;
	
};

