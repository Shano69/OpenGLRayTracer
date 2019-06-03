#include "Box.h"



bool Box::hit(const Ray & r, float tmin1, float tmax1, hit_record & rec) const
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[r.sign[0]].x - r.origin().x) * r.invdir.x;
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

	if ((tmin < tmax1) && (tmax > tmin1))
	{
		rec.t = tmax;
		rec.p = r.point_at_parameter(rec.t);

		rec.mat_ptr = mat;
		auto c = (bounds[0] + bounds[1]) * 0.5f;
		auto pi = rec.p - c;
		auto d = (bounds[0] - bounds[1]) * 0.5f;
		auto bias = 1.000001;

		rec.normal = -normalize(vec3(((pi.x / abs(d.x) * bias)),
			((pi.y / abs(d.y) * bias)),
			((pi.z / abs(d.z) * bias))));
		return true;
	}
	return false;
}

bool Box::bounding_box(float tmin, float tmax, AABB & box) const
{
	box = AABB(bounds[0],bounds[1]);
	return true;
}

glm::vec3 Box::getNormal(glm::vec3 p)
{
		auto c = (bounds[0] + bounds[1]) * 0.5f;
		auto pi = p - c;
		auto d = (bounds[0] - bounds[1]) * 0.5f;
		auto bias = 1.000001;

		auto result = vec3(((p.x / abs(d.x) * bias)),
			((p.y / abs(d.y) * bias)),
			((p.z / abs(d.z) * bias)));
		return result;
}
