#include "light.h"

static light_t light;

void init_light(vec3_t direction)
{
	light.direction = direction;
}

vec3_t get_Light_direction()
{
	return light.direction;
}

olc::Pixel light_apply_intensity(olc::Pixel original_color, float percentage_factor)
{
	olc::Pixel p;

	if (percentage_factor < 0) percentage_factor = 0;
	if (percentage_factor > 1) percentage_factor = 1;

	p.a = original_color.a;
	p.r = original_color.r * percentage_factor;
	p.g = original_color.g * percentage_factor;
	p.b = original_color.b * percentage_factor;

	return p;
}
