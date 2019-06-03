#ifndef HITABLE
#define HITABLE

#define _USE_MATH_DEFINES
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Ray.h"
#include "AABB.h"

class RayMaterial;
class RayTexture;

using namespace glm;

struct hit_record {

	float t;
	float u;
	float v;
	vec3 p;
	vec3 normal;
	RayMaterial *mat_ptr;
};

class Hitable
{
public:
	//record sphere ray intersection data
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, AABB& box) const = 0;
};

class flip_normals : public Hitable {
public:
	flip_normals(Hitable *p) : ptr(p) {}
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
		if (ptr->hit(r, t_min, t_max, rec)) {
			rec.normal = -rec.normal;
			return true;
		}
		else
			return false;
	}
	virtual bool bounding_box(float t0, float t1, AABB& box) const {
		return ptr->bounding_box(t0, t1, box);
	}
	Hitable *ptr;
};




#endif // !HITABLE