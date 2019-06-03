#pragma once
#include "Hitable.h"
#include "Triangle.h"
#include "HitableList.h"
#include "BVH.h"

class Box :
	public Hitable
{
public:
	Box() {};
	Box(const glm::vec3& vmin, const glm::vec3& vmax, RayMaterial *m) {
		bounds[0] = vmin;
		bounds[1] = vmax;
		mat = m;
	}
	~Box() {};
	glm::vec3 min() const { return bounds[0]; }
	glm::vec3 max() const { return bounds[1]; }
	virtual bool hit(const Ray& r, float tmin1, float tmax1, hit_record& rec)const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
	glm::vec3 getNormal(glm::vec3 p);

	glm::vec3 bounds[2];
	RayMaterial *mat;

};

