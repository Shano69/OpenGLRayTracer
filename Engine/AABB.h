#pragma once

#include "Ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class AABB
{
public:
	AABB() {};
	AABB(const glm::vec3& vmin, const glm::vec3& vmax) {
		bounds[0] = vmin;
		bounds[1] = vmax;
	}
	~AABB() {};
	glm::vec3 min() const { return bounds[0]; }
	glm::vec3 max() const { return bounds[1]; }
	bool hit(const Ray& r, float tmin, float tmax)const;
	

	glm::vec3 bounds[2];
};

AABB surrounding_box(AABB box0, AABB box1);