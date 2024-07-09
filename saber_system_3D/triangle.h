#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "Vector.h"
#include "olcPixelGameEngine.h"
#include <Utility>
#include "Graphics.h"
#include "texture.h"
#include "depthdrawer.h"
#include <limits>

typedef struct
{
	int a;
	int b;
	int c;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	olc::Pixel color;
	bool isVisible;
}face_t;


typedef struct
{
	vec4_t points[3];
	tex2_t texcoords[3];
	olc::Pixel p;
	olc::Sprite* texture;
	
}triangle_t;

void int_swap(int* v1, int* v2);
void float_swap(float* v1, float* v2);

vec3_t get_triangle_normal(vec4_t vertices[3]);

////////////parallel rasterization /////////////////////////////////////
void draw_triangle_fill(DepthDrawer& depth_draw, olc::PixelGameEngine* pge,
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	olc::Pixel color);

int edge_cross(vec2_t* a, vec2_t* b, vec2_t* p);

bool is_top_left(vec2_t* start, vec2_t* end);

////////////////////////rasteraization ////////////////////////////////////
//flat shading
void draw_filled_triangle(DepthDrawer& depth_draw, olc::PixelGameEngine* pge,
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	olc::Pixel color);

void draw_triangle_pixel(DepthDrawer& depth_draw, olc::PixelGameEngine* pge,
	int x, int y, olc::Pixel color,
	vec4_t point_a, vec4_t point_b, vec4_t point_c);



vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

void draw_triangle_texel(DepthDrawer& depth_draw, olc::PixelGameEngine* pge,
	int x, int y, olc::Sprite* sprite,
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);

void draw_textured_triangle(DepthDrawer& depth_draw, olc::PixelGameEngine* pge,
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2, olc::Sprite* sprite);

//gouraud shading
void draw_filled_triangle_gouraud(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, int x2, int y2, std::vector<olc::Pixel> colors);
void fill_flat_bottom_triangle_gouraud(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, int x2, int y2, std::vector<olc::Pixel> colors);
void fill_flat_top_triangle_gouraud(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, int x2, int y2, std::vector<olc::Pixel> colors);
#endif // !TRIANGLE_H


