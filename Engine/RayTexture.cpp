#include "RayTexture.h"

glm::vec3 constant_texture::value(float u, float v, const glm::vec3 & p) const
{
	return colour;
}

glm::vec3 checker_texture::value(float u, float v, const glm::vec3 & p) const
{
	float sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
	if (sines < 0)
	{
		return odd->value(u, v, p);
	}
	else
	{
		return even->value(u, v, p);
	}
}

glm::vec3 noise_texture::value(float u, float v, const glm::vec3 & p) const
{
	return glm::vec3(1, 1, 1) * 0.5f * (1 + sin(scale*p.z + 10* noise.turb(p)));
}
