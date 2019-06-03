#include "TexCreate.h"
#include <omp.h>
#include <thread>

#define _CRT_SECURE_NO_WARNINGS

TexCreate::TexCreate()
{

	da_sampler.samples = new glm::vec3[400 * 400];
	da_sampler.samplesppx = 0;
	cam.setMoved(false);
}


TexCreate::~TexCreate()
{
}

void TexCreate::createImage(int h, int w)
{


	// Seed with real random number if available
	random_device r;
	// Create random number generator
	default_random_engine e(r());
	// Create a distribution - we want doubles between 0.0 and 1.0
	uniform_real_distribution<double> distribution(0.0, 1.0);

	const int width = 180;
	const int height = 180;

	GLubyte imageData[width][height][4];

	int i, j, c;
	int ns = 1;


	glm::vec3 lower_left_corner(-1.0, -1.0, -1.0);
	glm::vec3 horizontal(2.0, 0.0, 0.0);
	glm::vec3 vertical(0.0, 2.0, 0.0);
	glm::vec3 origin(0.0, 0.0, 0.0);
	da_sampler.samplesppx++;

	Hitable *list[6];
	list[0] = new Sphere(vec3(0.3, 2.5, 2.5), 0.9, new lambertian(vec3(0.5,0.2,0.3)));
	list[1] = new Sphere(vec3(-100.5, 0, -1), 100, new lambertian(vec3(0.8, 0.8, 0.2)));
	list[2] = new Sphere(vec3(0.9, 2.5, -2.5), 1.5, new metal(vec3(0.4, 0.4, 0.4), 0.7));
	list[3] = new Sphere(vec3(0.6, -2.5, 2.5), 1.2, new metal(vec3(0.4, 0.4, 0.4), 0.1));
	list[4] = new Sphere(vec3(0.5, -2.5, -2.5), 1.0, new dielectric(1.5));
	list[5] = new Sphere(vec3(0.5, -2.5, -2.5), -0.98, new dielectric(1.5));

	Hitable *world = new HitableList(list, 6);
	vec3 lookfrom(5, 5, 9);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;
	
	// cam = RayCamera(lookfrom, lookat, vec3(0, 1, 0), 20, float(width) / float(height), aperture, dist_to_focus);
	if (cam.getCameraMoved())
	{
		da_sampler.samples = new glm::vec3[400 * 400];
		da_sampler.samplesppx = 0;
		cam.setMoved(false);
	}

	//attempt at parallel

	auto num_threads = thread::hardware_concurrency();

	//loops for each pixel
		for (i = 0; i < width; i++)
		{
			for (j = 0; j < height; j++)
			{
				vec3 col = vec3(0.0);

				//antialiasing by sampling
				for (int s = 0; s < ns; s++)
				{
					//add distribution(e) to make it sample surrounding points
					float u = float(i + distribution(e)) / float(width);
					float v = float(j + distribution(e)) / float(height);
					Ray r = cam.get_ray(u, v);
					vec3 p = r.point_at_parameter(2.0);
					col += colour(r, world, 0);
				}

				col /= float(ns);
				col = vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z));
				
				//store sample
				da_sampler.samples[i + j * width] += col;
				glm::vec3 c = da_sampler.samples[i + j * width] / static_cast<float>(da_sampler.samplesppx);


				imageData[j][i][0] = (GLubyte)int(255.99 * c[0]);
				imageData[j][i][1] = (GLubyte)int(255.99 * c[1]);
				imageData[j][i][2] = (GLubyte)int(255.99 * c[2]);
				imageData[j][i][3] = (GLubyte)int(255.99);
			}
		}
	

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &da_texture);
	glBindTexture(GL_TEXTURE_2D, da_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);



	// unbind image
	glBindTexture(GL_TEXTURE_2D, 0);
}


glm::vec3 TexCreate::colour(const Ray & r, Hitable *world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec))
	{
		Ray scattered;
		vec3 attenuation;
		if (depth < 10 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * colour(scattered, world, depth + 1);
		}
		else
		{
			return vec3(0, 0, 0);
		}
	}
	else
	{
		vec3 unit_direction = normalize(r.direction());
		float t = 0.5*(unit_direction.y+1.0);
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}



void TexCreate::clearSampler(bool * keys)
{
	for (int key = 0; key < 1024; key++)
	{
		if (keys[key])
		{
			for (unsigned int j = 0; j < 180 * 180; j++)
			{
				da_sampler.samples[j] = glm::vec3(0.0);
			}
			da_sampler.samplesppx = 0;
		}
	}
}

void TexCreate::mouseMoved()
{
	for (unsigned int j = 0; j < 180 * 180; j++)
	{
		da_sampler.samples[j] = glm::vec3(0.0);
	}
	da_sampler.samplesppx = 0;
}

void TexCreate::bind()
{
	glBindTexture(GL_TEXTURE_2D, da_texture);

}
