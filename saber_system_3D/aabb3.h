#ifndef AABB3_H
#define AABB3_H
#include "Vector.h"
#include <numeric>
#include "IntersectData.h"

class AABB3
{
public:
	AABB3(const vec3_t& minExtents, const vec3_t& maxExtents);

	IntersectData IntersectAABB3(const AABB3& other) const;

	inline const vec3_t& GetMinExtents() const;
	inline const vec3_t& GetMaxExtents() const;

private:
	const vec3_t m_minExtents;
	const vec3_t m_maxExtents;

};
#endif // !AABB3_h

