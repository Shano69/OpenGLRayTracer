#pragma once
#include "Ray.h"
#include "HitableList.h"
#include <random>

class RayMaterial
{

public:
	RayMaterial();
	~RayMaterial();

	//create reflection ray
	static vec3 reflect(const vec3& v, const vec3& n);
	//create refraction ray
	static bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
	{
		vec3 uv = normalize(v);
		float dt = dot(uv, n);
		float discriminant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);
		if (discriminant > 0)
		{
			refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
			return true;
		}
		else false;
	}
	//scatter on ray intersection
	virtual bool scatter(const Ray& r_in, const hit_record& rec, vec3& attenuation, Ray& scattered) const = 0;
	//schlick effect for dielectrics
	static float shlick(float cosine, float ref_idx)
	{
		float r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0)*pow((1 - cosine), 5);
	}

	static vec3 random_in_unit_sphere()
	{
		// Seed with real random number if available
		std::random_device r;
		// Create random number generator
		std::default_random_engine e(r());
		// Create a distribution - we want doubles between 0.0 and 1.0
		std::uniform_real_distribution<double> distribution(0.0, 1.0);

		vec3 p;
		do {
			p = 2.0f*vec3(distribution(e), distribution(e), distribution(e)) - vec3(1, 1, 1);
		} while (pow(glm::length(p),2) >= 1.0f);
		return p;
	}
};

class lambertian : public RayMaterial
{
public: 
	lambertian(const vec3& a) :albedo(a) {}
	//scatter in a random direction; no reflection
	virtual bool scatter(const Ray& r_in, const hit_record& rec, vec3& attenuation, Ray& scattered) const;
	vec3 albedo;
};

class metal : public RayMaterial
{
public:
	//scatter reflected rays
	metal(const vec3& a, float f) :albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const Ray& r_in, const hit_record& rec, vec3& attenuation, Ray& scattered) const;


	vec3 albedo;
	float fuzz;
};

class dielectric : public RayMaterial
{
public:
	//scatter reflected and refracted rays
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const Ray& r_in, const hit_record& rec, vec3& attenuation, Ray& scattered) const;

	float ref_idx;

};