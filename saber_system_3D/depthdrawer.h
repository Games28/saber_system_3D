#ifndef DEPTHDRAWER_H
#define DEPTHDRAWER_H
#include "olcPixelGameEngine.h"

class DepthDrawer
{
public:
	float* z_buffer = nullptr;
	olc::PixelGameEngine* pgePtr = nullptr;

public:
	DepthDrawer();
	~DepthDrawer();

	void Init(olc::PixelGameEngine* gfx);

	int ScreenWidth();
	int SCreenHeight();

	float get_z_buffer(int x, int y);

	void set_z_buffer(float depth, int x, int y);

	void Reset();


};

#endif // !DEPTHDRAWER_H


