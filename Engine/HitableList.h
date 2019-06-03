#ifndef HITABLE_LIST
#define HITABLE_LIST


#include "Hitable.h"

class HitableList :public Hitable
{
public:
	HitableList() {};
	HitableList(Hitable **l, int n) { list = l; list_size = n; }
	virtual bool hit(const Ray &r, float tmin, float tmax, hit_record& rec)const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
	Hitable **list;
	int list_size;
};

#endif // !HITABLE_LIST