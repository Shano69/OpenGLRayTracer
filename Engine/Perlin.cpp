#include "Perlin.h"

float *Perlin::ranfloat = perlin_generate();
int *Perlin::perm_x = perlin_generate_perm();
int *Perlin::perm_y = perlin_generate_perm();
int *Perlin::perm_z = perlin_generate_perm();


inline float trilinear_interp(float c[2][2][2], float u, float v, float w)
{
	float accum = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				accum += (i*u + (1 - i)*(1 - u)) * 
						(j*v + (1 - j)*(1 - v))	* 
						(k*w + (1 - k)*(1 - w)) * 
						c[i][j][k];
			}
		}
	}
	return accum;
}

float Perlin::noise(const glm::vec3 & p) const
{
	float u = p.x - floor(p.x);
	float v = p.y - floor(p.y);
	float w = p.z - floor(p.z);

	u = u * u*(3 - 2 * u);
	v = v * v* (3 - 2 * v);
	w = w * w *(3 - 2 * w);

	int i = floor(p.x);
	int j = floor(p.y);
	int k = floor(p.z);

	float c[2][2][2];
	for (int di = 0; di < 2; di++)
	{
		for (int dj = 0; dj < 2; dj++)
		{
			for (int dk = 0; dk < 2; dk++)
			{
				c[di][dj][dk] = ranfloat[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
			}
		}
	}

	return trilinear_interp(c, u, v, w);
}

float * Perlin::perlin_generate()
{
	// Seed with real random number if available
	std::random_device r;
	// Create random number generator
	std::default_random_engine e(r());
	// Create a distribution - we want doubles between 0.0 and 1.0
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	float * p = new float[256];
	for (int i = 0; i < 256; ++i)
	{
		p[i] = distribution(e);
	}
	return p;
}

void permute(int * p, int n)
{
	// Seed with real random number if available
	std::random_device r;
	// Create random number generator
	std::default_random_engine e(r());
	// Create a distribution - we want doubles between 0.0 and 1.0
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	for (int i = n - 1; i > 0; i--)
	{
		int target = int(distribution(e) * (i + 1));
		int tmp = p[i];
		p[i] = p[target];
		p[target] = tmp;
	}
}

int * Perlin::perlin_generate_perm()
{
	int * p = new int[256];
	for (int i = 0; i < 256; i++)
	{
		p[i] = i;
		
	}
	permute(p, 256);
	return p;
}

float Perlin::turb(const glm::vec3 & p, int depth) const
{
	float accum = 0;
	glm::vec3 temp_p = p;
	float weight = 1.0;

	for (int i = 0; i < depth; i++)
	{
		accum += weight * noise(temp_p);
		weight *= 0.5;
		temp_p *= 2;
	}

	return fabs(accum);
}
