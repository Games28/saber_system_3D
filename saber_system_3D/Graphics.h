#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "olcPixelGameEngine.h"
#include "Vector.h"
#include "light.h"

enum Cull_Method
{
	CULL_NONE,
	CULL_BACKFACE
};

enum Render_Method
{
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE
};


void draw_line(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, olc::Pixel color);
void draw_line_gouraud(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, olc::Pixel color0,olc::Pixel color1);
void draw_line_phong(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, olc::Pixel color,vec3_t normal0,vec3_t noral1);


void draw_rect(olc::PixelGameEngine* pge, int x, int y, int width, int height, olc::Pixel color);
void draw_triangle(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, int x2, int y2, olc::Pixel color);

float lerp_float(float a, float b, float f);
vec3_t lerp_vec3(vec3_t a, vec3_t b, float f);
float inverse_lerp(float a, float b, float value);
olc::Pixel lerp_color(olc::Pixel a, olc::Pixel b, float f);
olc::Pixel lerp_color_sRGB(olc::Pixel color1, olc::Pixel color2, float fraction);
olc::Pixel multiply_colors(olc::Pixel color1, olc::Pixel color2);
olc::Pixel add_colors(olc::Pixel color1, olc::Pixel color2);
olc::Pixel subtract_colors(olc::Pixel color1, olc::Pixel color2);



#endif // !GRAPHICS_H


