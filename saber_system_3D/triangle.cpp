#include "triangle.h"

void int_swap(int* v1, int* v2)
{
	
	int temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}
void float_swap(float* v1, float* v2)
{
	float temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}
vec3_t get_triangle_normal(vec4_t vertices[3])
{
	//backface culling test to see if the current face should be projected
	vec3_t vector_a = vec3_from_vec4(vertices[0]); /*    A     */
	vec3_t vector_b = vec3_from_vec4(vertices[1]); /*  /   \      */
	vec3_t vector_c = vec3_from_vec4(vertices[2]); /* B-----C      */

	//get the vector subtraction of A-B and A-C
	vec3_t vector_ab = vec3_sub(vector_b, vector_a);
	vec3_t vector_ac = vec3_sub(vector_c, vector_a);
	vec3_normalize(&vector_ab);
	vec3_normalize(&vector_ac);

	//compute the face normal(using the cross project to find perpendicular vector)
	vec3_t normal = vec3_cross(vector_ab, vector_ac);
	vec3_normalize(&normal);

	return normal;
}
void draw_triangle_fill(DepthDrawer& depth_draw, olc::PixelGameEngine* pge, int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, olc::Pixel color)
{
	int x_min = std::min(std::min(x0, x1), x2);
	int y_min = std::min(std::min(y0, y1), y2);
	int x_max = std::max(std::max(x0, x1), x2);
	int y_max = std::max(std::max(y0, y1), y2);


	vec2_t v0 = vec2_new(x0, y0);
	vec2_t v1 = vec2_new(x1, y1);
	vec2_t v2 = vec2_new(x2, y2);


	float area = edge_cross(&v0, &v1, &v2);

	int bias0 = is_top_left(&v1, &v2) ? 0 : -1;
	int bias1 = is_top_left(&v2, &v0) ? 0 : -1;
	int bias2 = is_top_left(&v0, &v1) ? 0 : -1;


	for (int y = y_min; y <= y_max; y++)
	{
		for (int x = x_min; x <= x_max; x++)
		{
			vec2_t p = { x,y };

			int w0 = edge_cross(&v1, &v2, &p) + bias0;
			int w1 = edge_cross(&v2, &v0, &p) + bias1;
			int w2 = edge_cross(&v0, &v1, &p) + bias2;

			bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;

			if (is_inside)
			{

				pge->Draw(x, y, color);
			}
		}
	}

}
int edge_cross(vec2_t* a, vec2_t* b, vec2_t* p)
{
	vec2_t ab = { b->x - a->x, b->y - a->y };
	vec2_t ap = { p->x - a->x, p->y - a->y };

	return ab.x * ap.y - ab.y * ap.x;
}
bool is_top_left(vec2_t* start, vec2_t* end)
{
	vec2_t edge = { end->x - start->x, end->y - start->y };
	bool is_top_edge = edge.y == 0 && edge.x > 0;
	bool is_left_edge = edge.y < 0;

	return  is_top_edge || is_left_edge;
}
///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(DepthDrawer& depth_draw, olc::PixelGameEngine* pge, 
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	olc::Pixel color)
{
	
	// We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}

	// Create three vector points after we sort the vertices
	vec4_t point_a = { x0, y0, z0, w0 };
	vec4_t point_b = { x1, y1, z1, w1 };
	vec4_t point_c = { x2, y2, z2, w2 };

	///////////////////////////////////////////////////////
	// Render the upper part of the triangle (flat-bottom)
	///////////////////////////////////////////////////////
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0) {
		for (int y = y0; y <= y1; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
			}

			for (int x = x_start; x < x_end; x++) {
				// Draw our pixel with a solid color
				draw_triangle_pixel(depth_draw, pge, x, y, color, point_a, point_b, point_c);
			}
		}
	}

	///////////////////////////////////////////////////////
	// Render the bottom part of the triangle (flat-top)
	///////////////////////////////////////////////////////
	inv_slope_1 = 0;
	inv_slope_2 = 0;

	if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y2 - y1 != 0) {
		for (int y = y1; y <= y2; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
			}

			for (int x = x_start; x < x_end; x++) {
				// Draw our pixel with a solid color
				draw_triangle_pixel(depth_draw, pge, x, y, color, point_a, point_b, point_c);
			}
		}
	}
}


void draw_triangle_pixel(DepthDrawer& depth_draw, olc::PixelGameEngine* pge, int x, int y, olc::Pixel color, vec4_t point_a, vec4_t point_b, vec4_t point_c)
{
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);

	// Calculate the barycentric coordinates of our point 'p' inside the triangle
	vec3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// Interpolate the value of 1/w for the current pixel
	float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	// Adjust 1/w so the pixels that are closer to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	float currentdepth = depth_draw.get_z_buffer(x, y);


	if (interpolated_reciprocal_w < currentdepth)
	{
		pge->Draw(x, y, color);


		depth_draw.set_z_buffer(interpolated_reciprocal_w,x, y);
	}



}



vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
	vec2_t ac = vec2_sub(c, a);
	vec2_t ab = vec2_sub(b, a);
	vec2_t ap = vec2_sub(p, a);
	vec2_t pc = vec2_sub(c, p);
	vec2_t pb = vec2_sub(b, p);

	// Compute the area of the full parallegram/triangle ABC using 2D cross product
	float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||

	// Alpha is the area of the small parallelogram/triangle PBC divided by the area of the full parallelogram/triangle ABC
	float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;

	// Beta is the area of the small parallelogram/triangle APC divided by the area of the full parallelogram/triangle ABC
	float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

	// Weight gamma is easily found since barycentric coordinates always add up to 1.0
	float gamma = 1 - alpha - beta;

	vec3_t weights = { alpha, beta, gamma };
	return weights;
}

void draw_triangle_texel(DepthDrawer& depth_draw, olc::PixelGameEngine* pge, int x, int y, olc::Sprite* sprite,
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);
	vec3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	//variables to store the inperpolated values of u ,v and 1/w for correct pixel
	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w;

	// Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
	interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
	interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

	// also interpolate the value of 1/w for the current pixel
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + ( 1 / point_b.w) * beta + ( 1 / point_c.w) * gamma;


	// now we can divide back both interpolated values by 1/w
	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	// Map the UV coordinate to the full texture width and height

	int texture_width = sprite->width;
	int texture_height = sprite->height;

	size_t tex_x = (size_t)(interpolated_u * (float)texture_width) % texture_width;
	size_t tex_y = (size_t)(interpolated_v * (float)texture_height) % texture_height;
	
	// Adjust 1/w so the pixels that are closer to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;
	//depth draw test
	float currentdepth = depth_draw.get_z_buffer(x, y);
	
	
	if (interpolated_reciprocal_w < currentdepth)
	{
		pge->Draw(x, y, sprite->GetPixel(tex_x, tex_y));
	
		
		depth_draw.set_z_buffer(interpolated_reciprocal_w, x, y);
	}
	





}

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(DepthDrawer& depth_draw, olc::PixelGameEngine* pge,
	int x0, int y0,float z0, float w0, float u0, float v0,
	int x1, int y1,float z1, float w1, float u1, float v1, 
	int x2, int y2,float z2, float w2, float u2, float v2, olc::Sprite* sprite)
{
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}

	v0 = 1.0 - v0;
	v1 = 1.0 - v1;
	v2 = 1.0 - v2;

	vec4_t point_a = { (float)x0, (float)y0, z0,w0 };
	vec4_t point_b = { (float)x1, (float)y1, z1,w1 };
	vec4_t point_c = { (float)x2, (float)y2,z2,w2 };

	tex2_t a_uv = { u0,v0 };
	tex2_t b_uv = { u1, v1 };
	tex2_t c_uv = { u2,v2 };

	///////////////////////////////////////////////////////
	// Render the upper part of the triangle (flat-bottom)
	///////////////////////////////////////////////////////
	float inv_slope_1 = 0;
	float inv_slope_2 = 0;

	if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0) {
		for (int y = y0; y <= y1; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
			}

			for (int x = x_start; x < x_end; x++) {
				// Draw our pixel with the color that comes from the texture
				draw_triangle_texel(depth_draw,pge, x, y, sprite, point_a, point_b, point_c, a_uv, b_uv,c_uv);
			}
		}
	}

	///////////////////////////////////////////////////////
	// Render the bottom part of the triangle (flat-top)
	///////////////////////////////////////////////////////
	inv_slope_1 = 0;
	inv_slope_2 = 0;

	if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y2 - y1 != 0) {
		for (int y = y1; y <= y2; y++) {
			int x_start = x1 + (y - y1) * inv_slope_1;
			int x_end = x0 + (y - y0) * inv_slope_2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
			}

			for (int x = x_start; x < x_end; x++) {
				// Draw our pixel with the color that comes from the texture
				draw_triangle_texel(depth_draw,pge, x, y, sprite, point_a, point_b, point_c, a_uv, b_uv, c_uv);
			}
		}
	}
}


