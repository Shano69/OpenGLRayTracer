#pragma once

#include "Hitable.h"
#include <random>
#include<iostream>

class BVH :public Hitable
{
public:
	BVH();
	BVH(Hitable **l, int n, float time0, float time1);
	
	~BVH();
	
	virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;

	

	Hitable *left;
	Hitable *right;
	AABB box;
};



int box_x_compare(const void * a, const void * b);

int box_y_compare(const void * a, const void * b);

int box_z_compare(const void * a, const void * b);
