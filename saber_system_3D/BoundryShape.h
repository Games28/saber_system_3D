#ifndef BOUNDRYSHAPE_H
#define BOUNDRYSHAPE_H
#include "Vector.h"


enum ShapeType
{
	BCIRCLE,
	BPOLYGON,
	BBOX
};
struct BoundryShape
{
	virtual ~BoundryShape() = default;
	virtual ShapeType GetType() const;

};

struct BoundryCircle : public BoundryShape
{
	float radius;

	BoundryCircle(const float radius);
	virtual ~BoundryCircle();
	ShapeType GetType() const override;
};

struct BoundryPolygon : public BoundryShape
{
	std::vector<vec3_t*> vertices;

	BoundryPolygon(std::vector<vec3_t*> vertices);
	virtual ~BoundryPolygon();
	ShapeType GetType() const override;
};

struct BoundryBox : public BoundryPolygon
{
	
};

#endif // !BOUNDRYSHAPE_H






