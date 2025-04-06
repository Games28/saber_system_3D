#ifndef RIGIDSURFACE_H
#define RIGIDSURFACE_H
#include "olcPixelGameEngine.h"
#include "Vector.h"

class RigidSurface
{
public:
	RigidSurface(const std::vector<vec3_t>* points, const vec3_t normalVector, bool interiorsurface = false);
	RigidSurface(const RigidSurface& surface);
	std::vector<vec3_t*> GetPoints();
	vec3_t GetUnitNorm();
	float getInverseSegmentMagnitude(int pointindex);
	bool isInteriorSurface();
	
private:
	std::vector<vec3_t*> points;
	std::vector<float> inverseSegmentMagnitudes;
	float nVInverseMag;
	bool interiorSurface;
	vec3_t getNCross();

	void caclulateInverseSegmentMagnitude();
	void reverseOrder();

};


#endif // !RIGIDSURFACE_H


