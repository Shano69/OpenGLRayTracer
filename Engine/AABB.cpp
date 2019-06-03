#include "AABB.h"

bool AABB::hit(const Ray & r, float timemin, float timemax) const
{

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[r.sign[0]].x - r.origin().x) *r.invdir.x;
	tmax = (bounds[1 - r.sign[0]].x - r.origin().x) * r.invdir.x;
	tymin = (bounds[r.sign[1]].y - r.origin().y) * r.invdir.y;
	tymax = (bounds[1 - r.sign[1]].y - r.origin().y) * r.invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[r.sign[2]].z - r.origin().z) * r.invdir.z;
	tzmax = (bounds[1 - r.sign[2]].z - r.origin().z) * r.invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

AABB surrounding_box(AABB box0, AABB box1)
{
	glm::vec3 small(fmin(box0.min().x, box1.min().x),
		fmin(box0.min().y, box1.min().y),
		fmin(box0.min().z, box1.min().z));
	glm::vec3 big(fmax(box0.max().x, box1.max().x),
		fmax(box0.max().y, box1.max().y),
		fmax(box0.max().z, box1.max().z));
	return AABB(small, big);
}
