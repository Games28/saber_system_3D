#pragma once
#ifndef AABB3_STRUCT_H
#define AABB3_STRUCT_H
#include "Vector.h"

struct AABB3 {
	vec3_t min{ INFINITY, INFINITY, INFINITY }, max = { -INFINITY, -INFINITY, -INFINITY };

	vec3_t getCenter() const {
		vec3_t add = vec3_add(min, max);
		vec3_t result = vec3_div(add, 2);
		return result;
	}

	bool contains(const vec3_t& p) const {
		if (p.x<min.x || p.x>max.x) return false;
		if (p.y<min.y || p.y>max.y) return false;
		if (p.z<min.z || p.z>max.z) return false;
		return true;
	}

	bool overlaps(const AABB3& o) const {
		if (min.x > o.max.x || max.x < o.min.x) return false;
		if (min.y > o.max.y || max.y < o.min.y) return false;
		if (min.z > o.max.z || max.z < o.min.z) return false;
		return true;
	}

	void fitToEnclose(const vec3_t& p) {
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;
		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}
};
#endif//AABB3_STRUCT_H
