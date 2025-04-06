#include "RIgidSurface.h"

RigidSurface::RigidSurface(const std::vector<vec3_t>* points, const vec3_t normalVector, bool interiorsurface)
{
    for (vec3_t p : *points)
    {
       
        this->points.push_back(new vec3_t{ p.x,p.y,p.z });
    }

}

RigidSurface::RigidSurface(const RigidSurface& surface)
{
}

std::vector<vec3_t*> RigidSurface::GetPoints()
{
    return std::vector<vec3_t*>();
}

vec3_t RigidSurface::GetUnitNorm()
{
    return vec3_t();
}

float RigidSurface::getInverseSegmentMagnitude(int pointindex)
{
    return 0.0f;
}

bool RigidSurface::isInteriorSurface()
{
    return false;
}

vec3_t RigidSurface::getNCross()
{
    return vec3_t();
}

void RigidSurface::caclulateInverseSegmentMagnitude()
{
}

void RigidSurface::reverseOrder()
{
}
