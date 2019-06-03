#pragma once

#include <glm/glm.hpp>
#include "Perlin.h"

class RayTexture
{
public:
	
	virtual glm::vec3 value(float u, float v, const glm::vec3& p) const = 0;
};

class constant_texture :public RayTexture
{
public:
	constant_texture() {};
	constant_texture(glm::vec3 c) :colour(c) { }
	virtual glm::vec3 value(float u, float v, const glm::vec3& p) const;


	glm::vec3 colour;
};

class checker_texture : public RayTexture
{
public:
	checker_texture() {};
	checker_texture(RayTexture* t0, RayTexture *t1) : even(t0), odd(t1) { };

	virtual glm::vec3 value(float u, float v, const glm::vec3& p) const;

	RayTexture *odd;
	RayTexture *even;
};

class noise_texture : public RayTexture
{
public:
	noise_texture() {}
	noise_texture(float sc) :scale(sc) {};
	virtual glm::vec3  value(float u, float v, const glm::vec3& p) const;

	Perlin noise;
	float scale;
};
