#ifndef SPHERE
#define SPHERE


#include "Hitable.h"


class Sphere :public Hitable
{
public:
	Sphere() {}
	Sphere(vec3 cen, float r,  RayMaterial *m) : center(cen), radius(r), mat_ptr(m) {};
	virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec)const;
	virtual bool Sphere::bounding_box(float t0, float t1, AABB& box) const;
	

	vec3 center;
	float radius;
	RayMaterial *mat_ptr;
};


#endif // !SPHERE