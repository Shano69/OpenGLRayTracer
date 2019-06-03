#include "Triangle.h"

bool Triangle::hit(const Ray & r, float tmin, float tmax, hit_record & rec) const
{
	//moller_trumbore optimization 
	vec3 v0v1 = _v1 - _v0;
	vec3 v0v2 = _v2 - _v0;
	vec3 pvec = cross(r.direction(), v0v2);
	float det = dot(v0v1, pvec);

	if (det < 0.0001) return false;

	float invDet = 1 / det;
	vec3 tvec = r.origin() - _v0;
	float tempu, tempv;

	tempu = dot(tvec, pvec) * invDet;
	if (tempu < 0 || tempu >1) return false;

	vec3 qvec = cross(tvec, v0v1);
	tempv = dot(r.direction(), qvec) * invDet;
	if (tempv < 0 || tempu + tempv >1) return false;
	
	rec.t = dot(v0v2, qvec) * invDet;
	rec.normal = normalize(cross(v0v1, v0v2));
	rec.p = r.point_at_parameter(rec.t);
	rec.mat_ptr = mat;
	rec.u = tempu;
	rec.v = tempv;
}

bool Triangle::bounding_box(float t0, float t1, AABB & box) const
{
	vec3 min = vec3(FLT_MAX), max = vec3(FLT_MIN);
	// create the smallest point
	for (int i = 0; i < 3; i++)
	{
		if (min[i] > _v0[i])
		{
			min[i] = _v0[i];
		}
		if (min[i] > _v1[i])
		{
			min[i] = _v1[i];
		}
		if (min[i] > _v2[i])
		{
			min[i] = _v2[i];
		}
		if (max[i] < _v0[i])
		{
			max[i] = _v0[i];
		}
		if (max[i] < _v1[i])
		{
			max[i] = _v1[i];
		}
		if (max[i] < _v2[i])
		{
			max[i] = _v2[i];
		}

	}
	box = AABB(min,max);
	return true;
}
