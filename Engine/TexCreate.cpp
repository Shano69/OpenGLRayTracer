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
	
	const int width = 300;
	const int height = 300;

	int i, j, c;
	int ns = 1;


	glm::vec3 lower_left_corner(-1.0, -1.0, -1.0);
	glm::vec3 horizontal(2.0, 0.0, 0.0);
	glm::vec3 vertical(0.0, 2.0, 0.0);
	glm::vec3 origin(0.0, 0.0, 0.0);
	da_sampler.samplesppx++;

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
				//add distribution(e) to make it sample surrounding points
				float u = float(i + distribution(e)) / float(width);
				float v = float(j + distribution(e)) / float(height);
				Ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += colour(r, world, 0);

				col /= float(ns);
				col = vec3(sqrt(col.x), sqrt(col.y), sqrt(col.z));
				

				//BUG begone!!!
				if (!(col.x == col.x) )col = glm::vec3(0);
				if(!(col.y == col.y))col = glm::vec3(0);
				if(!( col.z == col.z))col = glm::vec3(0);


				//store sample
				da_sampler.samples[i + j * width] += col;
				glm::vec3 c = da_sampler.samples[i + j * width] / static_cast<float>(da_sampler.samplesppx);

				if (c[0] > 1)
				{
					imageData[j][i][0] = (GLubyte)int(255.99 );
				}
				else
				{
					imageData[j][i][0] = (GLubyte)int(255.99 * c[0]);
				}
				if (c[1] > 1)
				{
					imageData[j][i][1] = (GLubyte)int(255.99);
				}
				else
				{
					imageData[j][i][1] = (GLubyte)int(255.99 * c[1]);
				}
				if (c[2] > 1)
				{
					imageData[j][i][2] = (GLubyte)int(255.99);
				}
				else
				{
					imageData[j][i][2] = (GLubyte)int(255.99 * c[2]);
				}
				
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

void TexCreate::createWorld()
{

	// Seed with real random number if available
	random_device r;
	// Create random number generator
	default_random_engine e(r());
	// Create a distribution - we want doubles between 0.0 and 1.0
	uniform_real_distribution<double> distribution(0.0, 1.0);
	

	RayTexture *checker = new checker_texture(new constant_texture(glm::vec3(0.65, 0.05, 0.05)), new constant_texture(glm::vec3(0.9, 0.9, 0.9)));
	RayTexture *pertex = new noise_texture( 4);
	RayMaterial *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	RayMaterial *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	RayMaterial *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	RayMaterial *m = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	RayMaterial *light = new diffuse_light(new constant_texture(vec3(30)));
	RayMaterial *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
	RayMaterial * aluminium = new metal(vec3(0.8, 0.85, 0.88), 0.0);

	//mirror
	vec3 v1 = vec3(25, 10, -15);
	vec3 v2 = vec3(25, 10, 15);
	vec3 v3 = vec3(25, 41, 15);
	vec3 v4 = vec3(25, 41, -15);


	//ground
	vec3 v5 = vec3(-30, 0, -20);
	vec3 v6 = vec3(30, 0, -20);
	vec3 v7 = vec3(30, 0, 20);
	vec3 v8 = vec3(-30, 0, 20);

	//another mirror
	vec3 v11 = vec3(-25, 10, -15);
	vec3 v22 = vec3(-25, 10, 15);
	vec3 v33 = vec3(-25, 41, 15);
	vec3 v44 = vec3(-25, 41, -15);


	Hitable **list = new Hitable*[500];
	int k = 0;
	
	list[k++] = new Box(vec3(-10, 40, -15), glm::vec3(10, 41, 45), light);
	list[k++] = new Box(vec3(-15, 5, -5), glm::vec3(-5, 15, 5), red);

	list[k++] = new Sphere(vec3(5.0, 8.0, 5.0), 5, new lambertian(pertex));
	list[k++] = new Sphere(vec3(5.0, 10.0, -5.5), 4, new metal(vec3(0.4, 0.4, 0.2), 0.3));
	list[k++] = new Sphere(vec3( -5, 15.5, 4.5), 4.5, new metal(vec3(0.7, 0.3, 0.2), 0.01));
	list[k++] = new Sphere(vec3( -4.5, 20.0, -6.0), 3.0, new dielectric(1.5));
	list[k++] = new Sphere(vec3( -4.5, 20.0, -6.0), -2.99, new dielectric(1.5));


	list[k++] = new Triangle(v1, v2, v3, aluminium);
	list[k++] = new Triangle(v1, v3, v4, aluminium);

	list[k++] = new Triangle(v22, v11, v33, aluminium);
	list[k++] = new Triangle(v33, v11, v44, aluminium);

	list[k++] = new Triangle(v6, v5, v7, white);
	list[k++] = new Triangle(v7, v5, v8, white);
	

	//world = new BVH(list,k,0,1);
	world = new BVH(list, k,0,1);
}


glm::vec3 TexCreate::colour(const Ray & r, Hitable *world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		Ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 5 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) 
		{	
			return emitted + attenuation * colour(scattered, world, depth + 1);
		}
		else {
				return emitted;
		}
	}
	else {
		vec3 unit_direction = normalize(r.direction());
		float t = 0.5*(unit_direction.y + 1.0);
		auto returnValue = (1.0 - t)*vec3(.1, 0.1, 0.1) + t * vec3(0.01, 0.02, 0.05);
		if (returnValue.x > 1.0 || returnValue.y > 1.0 || returnValue.z > 1.0)
		{
			return returnValue;
		}
		return returnValue;
	}
}



void TexCreate::clearSampler(bool * keys)
{
	for (int key = 0; key < 1024; key++)
	{
		if (keys[key])
		{
			
			da_sampler.samples = new glm::vec3[300*300];
			da_sampler.samplesppx = 0;
		}
	}
}

void TexCreate::mouseMoved()
{
	for (unsigned int j = 0; j < 300 * 300; j++)
	{
		da_sampler.samples[j] = glm::vec3(0.0);
	}
	da_sampler.samplesppx = 0;
}

void TexCreate::bind()
{
	glBindTexture(GL_TEXTURE_2D, da_texture);

}
