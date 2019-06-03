#include "Sphere.h"

bool Sphere::hit(const Ray & r, float tmin, float tmax, hit_record & rec) const
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < tmax && temp > tmin) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			glm::vec3 p = (rec.p - center) / radius;
			float phi = atan2(p.z, p.x);
			float theta = asin(p.y);
			rec.u = 1 - (phi + M_PI) / (2 * M_PI);
			rec.v = (theta + M_PI / 2) / M_PI;
			rec.normal = normalize(rec.p - center);
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < tmax && temp > tmin) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			glm::vec3 p = (rec.p - center) / radius;
			float phi = atan2(p.z, p.x);
			float theta = asin(p.y);
			rec.u = 1 - (phi + M_PI) / (2 * M_PI);
			rec.v = (theta + M_PI / 2) / M_PI;
			rec.normal = normalize(rec.p - center);
			rec.mat_ptr = mat_ptr;	
			return true;
		}
	}
	return false;
}

bool Sphere::bounding_box(float t0, float t1, AABB & box) const
{
	box = AABB(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

