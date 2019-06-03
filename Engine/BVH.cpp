#include "BVH.h"



BVH::BVH()
{
}


BVH::~BVH()
{
}

BVH::BVH(Hitable ** l, int n, float time0, float time1)
{
	// Seed with real random number if available
	std::random_device r;
	// Create random number generator
	std::default_random_engine e(r());
	// Create a distribution - we want doubles between 0.0 and 1.0
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	int axis = int(3 * distribution(e));
	if (axis == 0)
		qsort(l, n, sizeof(Hitable *), box_x_compare);
	else if (axis == 1)
		qsort(l, n, sizeof(Hitable *), box_y_compare);
	else
		qsort(l, n, sizeof(Hitable *), box_z_compare);
	if (n == 1) {
		left = right = l[0];
	}
	else if (n == 2) {
		left = l[0];
		right = l[1];
	}
	else {
		left = new BVH(l, n / 2, time0, time1);
		right = new BVH(l + n / 2, n - n / 2, time0, time1);
	}
	AABB box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
		std::cout << "no bounding box in bvh_node constructor\n" << std::endl;
	box = surrounding_box(box_left, box_right);
}

bool BVH::hit(const Ray & r, float tmin, float tmax, hit_record & rec) const
{
	if (box.hit(r, tmin, tmax)) 
	{
		hit_record left_rec, right_rec;

		bool hit_left = left->hit(r, tmin, tmax, left_rec);

		bool hit_right = right->hit(r, tmin, tmax, right_rec);

		if (hit_left && hit_right) 
		{
			if (left_rec.t < right_rec.t)
			{
				rec = left_rec;
			}
			else
			{
				rec = right_rec;
			}
			return true;
		}

		else if (hit_left) 
		{
			rec = left_rec;
			return true;
		}

		else if (hit_right) 
		{
			rec = right_rec;
			return true;
		}

		else
			return false;
	}
	else return false;
}

bool BVH::bounding_box(float t0, float t1, AABB & b) const
{
	b = box;
	return true;
}


int box_x_compare(const void * a, const void * b)
{
	{
		AABB box_left, box_right;
		Hitable *ah = *(Hitable**)a;
		Hitable *bh = *(Hitable**)b;
		if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
			std::cout << "no bounding box in bvh_node constructor" << std::endl;
		if (box_left.min().x - box_right.min().x < 0.0)
			return -1;
		else
			return 1;
	}
}

int box_y_compare(const void * a, const void * b)
{
	{
		AABB box_left, box_right;
		Hitable *ah = *(Hitable**)a;
		Hitable *bh = *(Hitable**)b;
		if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
			std::cout << "no bounding box in bvh_node constructor" << std::endl;
		if (box_left.min().y - box_right.min().y < 0.0)
			return -1;
		else
			return 1;
	}
}

int box_z_compare(const void * a, const void * b)
{
	{
		AABB box_left, box_right;
		Hitable *ah = *(Hitable**)a;
		Hitable *bh = *(Hitable**)b;
		if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
			std::cout << "no bounding box in bvh_node constructor" << std::endl;
		if (box_left.min().z - box_right.min().z < 0.0)
			return -1;
		else
			return 1;
	}

}
