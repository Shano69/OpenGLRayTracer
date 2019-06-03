#pragma once

#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <float.h>
#include <random>

#include "src/Application.h"
#include "Sphere.h"
#include "HitableList.h"
#include "RayCamera.h"
#include "RayMaterial.h"



#include <vector>

using namespace std;
using namespace glm;

struct sampler
{
	float samplesppx;
	glm::vec3 *samples;
};

class TexCreate
{
public:
	TexCreate();
	~TexCreate();

	void createImage(int h, int w);
	//image making 
	glm::vec3 colour(const Ray& r, Hitable *world, int depth);
	//update camera
	inline void setCamera(RayCamera camera) { cam = camera; }
	//reset sampler for keyboard events
	void clearSampler(bool * keys);
	//handle mouse camera movement
	void mouseMoved();
	//bind the texture
	void bind();

private:

	RayCamera cam;
	sampler da_sampler;
	GLuint da_texture;
	unsigned int width;
	unsigned int x ;
	unsigned int y ;
	unsigned int location;
};

