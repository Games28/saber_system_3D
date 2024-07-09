#include "depthdrawer.h"
#include <cfloat>
DepthDrawer::DepthDrawer() {}

DepthDrawer::~DepthDrawer()
{
	delete z_buffer;
}

void DepthDrawer::Init(olc::PixelGameEngine* gfx)
{
	pgePtr = gfx;
	z_buffer = (float*)malloc(sizeof(float) * pgePtr->ScreenWidth() * pgePtr->ScreenHeight());

	
}

int DepthDrawer::ScreenWidth()
{
	return pgePtr->ScreenWidth();
}

int DepthDrawer::SCreenHeight()
{
	return pgePtr->ScreenHeight();
}

float DepthDrawer::get_z_buffer(int x, int y)
{
	float result = 0.0f;
	if (x >= 0 && x < pgePtr->ScreenWidth() &&
		y >= 0 && y < pgePtr->ScreenHeight())
	{
		result = z_buffer[y * pgePtr->ScreenWidth() + x];
	}

	return result;
}

void DepthDrawer::set_z_buffer(float depth, int x, int y)
{

	if (x >= 0 && x < pgePtr->ScreenWidth() &&
		y >= 0 && y < pgePtr->ScreenHeight())
	{
		
		
			z_buffer[y * pgePtr->ScreenWidth() + x] = depth;
			
		
	}
}

void DepthDrawer::Reset()
{
	for (int y = 0; y < pgePtr->ScreenHeight(); y++)
	{
		for (int x = 0; x < pgePtr->ScreenWidth(); x++)
		{
			z_buffer[(pgePtr->ScreenWidth() * y) + x] = 1.0f;
		}
	}
}
