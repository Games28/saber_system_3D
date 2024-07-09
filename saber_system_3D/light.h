#ifndef LIGHT_H
#define LIGHT_H
#include "olcPixelGameEngine.h"
#include "Vector.h"

enum lighting_method
{
	NO_LIGHTING = 0,
	FLAT_SHADING = 1,
	GOURAUD_SHADING = 2,
	PHONG_SHADING = 3
};

typedef struct
{
	vec3_t direction;

}light_t;

void init_light(vec3_t direction);
vec3_t get_Light_direction();

olc::Pixel light_apply_intensity(olc::Pixel original_color, float percentage_factor);
#endif // !LIGHT_H


