#pragma once
#include "Hitable.h"
#include <cmath>
class Triangle :
	public Hitable
{
public:
	Triangle() {};
	Triangle(vec3 v0, vec3 v1, vec3 v2, RayMaterial *m)
		:_v0(v0), _v1(v1), _v2(v2), mat(m) {};
	~Triangle() {};
	virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec)const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;

	vec3 _v0, _v1, _v2;
	RayMaterial *mat;
};

