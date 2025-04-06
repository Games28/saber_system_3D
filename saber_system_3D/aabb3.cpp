#include "aabb3.h"

AABB3::AABB3(const vec3_t& minExtents, const vec3_t& maxExtents)
	: m_minExtents(minExtents), m_maxExtents(maxExtents)
{

}

IntersectData AABB3::IntersectAABB3(const AABB3& other) const
{
	vec3_t distances1 = vec3_sub(other.GetMinExtents(),m_maxExtents);
	vec3_t distances2 = vec3_sub(m_minExtents,other.GetMaxExtents());
	
	float dist1 = sqrtf(distances1.x * distances1.x + distances1.y * distances1.y + distances1.z * distances1.z);
	float dist2 = sqrtf(distances2.x * distances2.x + distances2.y * distances2.y + distances2.z * distances2.z);
	
	float maxDistance;
	if (dist1 > dist2)
	{
		maxDistance = dist1;
	}
	else
	{
		maxDistance = dist2;

	}

	return IntersectData(maxDistance < 0,maxDistance);

}

inline const vec3_t& AABB3::GetMinExtents() const
{
	return m_minExtents;
}

inline const vec3_t& AABB3::GetMaxExtents() const
{
	return m_maxExtents;
}
