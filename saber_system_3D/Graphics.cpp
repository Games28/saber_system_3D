#include "Graphics.h"



void draw_line(olc::PixelGameEngine* pge,int x0, int y0, int x1, int y1, olc::Pixel color)
{
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int side_length = abs(delta_x) > abs(delta_y) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)side_length;
	float y_inc = delta_y / (float)side_length;

	float current_x = (float)x0;
	float current_y = (float)y0;

	for (int i = 0; i <= side_length; i++)
	{
		pge->Draw(round(current_x), round(current_y),color);

		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_line_gouraud(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, olc::Pixel color0, olc::Pixel color1)
{
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int side_length = abs(delta_x) > abs(delta_y) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)side_length;
	float y_inc = delta_y / (float)side_length;

	float current_x = (float)x0;
	float current_y = (float)y0;

	for (int i = 0; i <= side_length; i++)
	{
		float lerp = inverse_lerp(x0, x1, current_x);
		olc::Pixel color = lerp_color(color0, color1, lerp);
		if (x0 == x1 && y0 == y1)
		{
			color = color0;
		}

		pge->Draw(round(current_x), round(current_y), color);

		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_line_phong(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, olc::Pixel color, vec3_t normal0, vec3_t normal1)
{
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int side_length = abs(delta_x) > abs(delta_y) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)side_length;
	float y_inc = delta_y / (float)side_length;

	float current_x = (float)x0;
	float current_y = (float)y0;

	

	for (int i = 0; i <= side_length; i++)
	{
		float lerp = inverse_lerp(x0, x1, current_x);
		vec3_t normal = lerp_vec3(normal0, normal1, lerp);
		if (x0 == x1 && y0 == y1)
		{
			normal = normal1;
		}
		float light_intensity_factor = -vec3_dot(normal, get_Light_direction());

		olc::Pixel p = light_apply_intensity(color, light_intensity_factor);

		pge->Draw(round(current_x), round(current_y), p);

		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_rect(olc::PixelGameEngine* pge, int x, int y, int width, int height, olc::Pixel color)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int current_x = x + i;
			int current_y = y + j;
			pge->Draw(current_x, current_y, color);
		}
	}
}

void draw_triangle(olc::PixelGameEngine* pge, int x0, int y0, int x1, int y1, int x2, int y2, olc::Pixel color)
{
	draw_line(pge, x0, y0, x1, y1, color);
	draw_line(pge, x1, y1, x2, y2, color);
	draw_line(pge, x2, y2, x0, y0, color);
}

float lerp_float(float a, float b, float f)
{
	return (a * (1.0f - f)) + (b * f);
}

vec3_t lerp_vec3(vec3_t a, vec3_t b, float f)
{
	vec3_t result;
	result.x = lerp_float(a.x, b.x, f);
	result.y = lerp_float(a.y, b.y, f);
	result.z = lerp_float(a.z, b.z, f);
	return result;
}

float inverse_lerp(float a, float b, float value)
{
	return (value - a) / (b - a);
}

olc::Pixel lerp_color(olc::Pixel a, olc::Pixel b, float f)
{
	olc::Pixel result;

	result.a = a.a * (1.0f - f) + b.a * (uint8_t)f;
	result.r = a.r * (1.0f - f) + b.r * (uint8_t)f;
	result.g = a.g * (1.0f - f) + b.g * (uint8_t)f;
	result.b = a.b * (1.0f - f) + b.b * (uint8_t)f;


	return result;
}

olc::Pixel lerp_color_sRGB(olc::Pixel color1, olc::Pixel color2, float fraction)
{
	olc::Pixel result;

	//convert to linear sRGB space
	float R1 = powf(color1.r / 255.0f, 2.2f);
	float G1 = powf(color1.g / 255.0f, 2.2f);
	float B1 = powf(color1.b / 255.0f, 2.2f);

	float R2 = powf(color2.r / 255.0f, 2.2f);
	float G2 = powf(color2.g / 255.0f, 2.2f);
	float B2 = powf(color2.b / 255.0f, 2.2f);

	//interpolate in linear sRGB space
	float R = (1.0f - fraction) * R1 + fraction * R2;
	float B = (1.0f - fraction) * B1 + fraction * B2;
	float G = (1.0f - fraction) * G1 + fraction * G2;

	//convert back to sRGB space
	result.r = powf(R, 1.0f / 2.2f) * 255.0f + 0.5f;
	result.g = powf(G, 1.0f / 2.2f) * 255.0f + 0.5f;
	result.b = powf(B, 1.0f / 2.2f) * 255.0f + 0.5f;

	return result;

}

olc::Pixel multiply_colors(olc::Pixel color1, olc::Pixel color2)
{
	olc::Pixel result;

	result.a = (color1.a * color2.a) / 255;
	result.r = (color1.r * color2.r) / 255;
	result.g = (color1.g * color2.g) / 255;
	result.b = (color1.b * color2.b) / 255;


	return result;
}

olc::Pixel add_colors(olc::Pixel color1, olc::Pixel color2)
{
	olc::Pixel result;

	result.a = color1.a + color2.a;
	result.r = color1.r + color2.r;
	result.g = color1.g + color2.g;
	result.b = color1.b + color2.b;

	if (result.a > 255) result.a = 255;
	if (result.r > 255) result.r = 255;
	if (result.g > 255) result.g = 255;
	if (result.b > 255) result.b = 255;

	return result;
}

olc::Pixel subtract_colors(olc::Pixel color1, olc::Pixel color2)
{
	olc::Pixel result;
	
	result.a = (color1.a > color2.a) ? color1.a - color2.a : 0;
	result.r = (color1.r > color2.r) ? color1.r - color2.r : 0;
	result.g = (color1.g > color2.g) ? color1.g - color2.g : 0;
	result.b = (color1.b > color2.b) ? color1.b - color2.b : 0;


	return result;
}
