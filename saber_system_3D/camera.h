#ifndef CAMERA_H
#define CAMERA_H
#include "Vector.h"
#include "matrix.h"

//typedef struct
//{
//	vec3_t position;
//	vec3_t direction;
//	vec3_t forward_velocity;
//	float yaw;
//	float pitch;
//}camera_t;

class Camera
{
public:
	Camera();
	~Camera();
	//void init_camera(vec3_t position, vec3_t direction);

	vec3_t get_camera_position();
	vec3_t get_camera_direction();
	vec3_t get_camera_forward_velocity();
	float get_camera_yaw();
	float get_camera_pitch();

	void set_new_camera_position(vec3_t position);
	void set_new_camera_direction(vec3_t direction);
	void set_new_camera_forward_velocity(vec3_t forward_velocity);

	void rotate_camera_yaw(float angle);
	void rotate_camera_pitch(float angle);

	vec3_t get_camera_lookat_target();

	void update_camera_position(vec3_t position);
	void update_camera_direction(vec3_t direction);
	void update_camera_forward_velocity(vec3_t forward_velocity);
private:
	vec3_t position;
	vec3_t direction;
	vec3_t forward_velocity;
	float yaw;
	float pitch;
};

#endif // !CAMERA_H
