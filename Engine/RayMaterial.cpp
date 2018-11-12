#include "RayMaterial.h"


RayMaterial::RayMaterial()
{
}


RayMaterial::~RayMaterial()
{
}

vec3 RayMaterial::reflect(const vec3 & v, const vec3 & n)
{
	return v - 2 * dot(v, n)*n;
}

bool metal::scatter(const Ray & r_in, const hit_record & rec, vec3 & attenuation, Ray & scattered) const
{
	vec3 reflected = reflect(normalize(r_in.direction()), rec.normal);
	scattered = Ray(rec.p, reflected+ fuzz * random_in_unit_sphere());
	attenuation = albedo;
	return (dot(scattered.direction(), rec.normal) > 0);
}

bool lambertian::scatter(const Ray & r_in, const hit_record & rec, vec3 & attenuation, Ray & scattered) const
{
	vec3 target = rec.p + rec.normal + random_in_unit_sphere();
	scattered = Ray(rec.p, target - rec.p);
	attenuation = albedo;
	return true;
}

bool dielectric::scatter(const Ray & r_in, const hit_record & rec, vec3 & attenuation, Ray & scattered) const
{
	// Seed with real random number if available
		std::random_device r;
	// Create random number generator
	std::default_random_engine e(r());
	// Create a distribution - we want doubles between 0.0 and 1.0
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	vec3 outward_normal;
	vec3 reflected = reflect(r_in.direction(), rec.normal);
	float ni_over_nt;
	attenuation = vec3(1.0, 1.0, 1.0);
	vec3 refracted;
	float reflect_prob;
	float cosine;

	if (dot(r_in.direction(), rec.normal) > 0) 
	{
		outward_normal = -rec.normal;
		ni_over_nt = ref_idx;
		cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
		cosine = sqrt(1 - ref_idx * ref_idx*(1 - cosine * cosine));
	}
	else 
	{
		outward_normal = rec.normal;
		ni_over_nt = 1.0 / ref_idx;
		cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
	}

	if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
		reflect_prob = shlick(cosine, ref_idx);
	else
		reflect_prob = 1.0;

	if (distribution(e) < reflect_prob)
		scattered = Ray(rec.p, reflected);
	else
		scattered = Ray(rec.p, refracted);

	return true;
}
