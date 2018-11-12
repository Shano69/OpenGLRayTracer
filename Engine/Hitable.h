#ifndef HITABLE
#define HITABLE


#include "Ray.h"

class RayMaterial;

using namespace glm;
struct hit_record {
	float t;
	vec3 p;
	vec3 normal;
	RayMaterial *mat_ptr;
};

class Hitable
{
public:
	//record sphere ray intersection data
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif // !HITABLE