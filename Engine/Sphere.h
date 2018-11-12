#ifndef SPHERE
#define SPHERE


#include "Hitable.h"
class Sphere :public Hitable
{
public:
	Sphere() {}
	Sphere(vec3 cen, float r,  RayMaterial *m) : center(cen), radius(r), mat_ptr(m) {};
	virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec)const;
	vec3 center;
	float radius;
	RayMaterial *mat_ptr;
};


#endif // !SPHERE