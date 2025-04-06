#include "camera.h"

static camera_t camera;



//Camera::Camera()
//{
//	position = vec3_new(0, 0, 0);
//	direction = vec3_new(0, 0, 0);
//	forward_velocity = vec3_new(0, 0, 0);
//	yaw = 0.0f;
//	pitch = 0.0f;
//}
//
//Camera::~Camera()
//{
//}

//void Camera::init_camera(vec3_t position, vec3_t direction)
//{
//	position = vec3_new(0, 0, 0);
//	direction = vec3_new(0, 0, 0);
//	forward_velocity = vec3_new(0, 0, 0);
//	yaw = 0.0f;
//	pitch = 0.0f;
//
//}

void init_camera()
{
	camera.direction = vec3_new(0, 0, 1);
	camera.strafe = vec3_new(1, 0, 0);
}

vec3_t get_camera_position()
{
	return camera.position;
}

vec3_t get_camera_direction()
{
	return camera.direction;
}

vec3_t get_camera_forward_velocity()
{
	return camera.forward_velocity;
}

vec3_t get_camera_strafe_velocity()
{
	return camera.strafe_velocity;
}

vec3_t get_camera_stafe()
{
	return camera.strafe;
}

float get_camera_yaw()
{
	return camera.yaw;
}

float get_camera_pitch()
{
	return camera.pitch;
}

void set_new_camera_position(vec3_t position)
{
	camera.position = position;
}

void set_new_camera_direction(vec3_t direction)
{
	camera.direction = direction;
}

void set_new_camera_forward_velocity(vec3_t forward_velocity)
{
	camera.forward_velocity = forward_velocity;
}

void set_new_camera_strafe_velocity(vec3_t strafe_velocity)
{
	camera.strafe_velocity = strafe_velocity;
}

void rotate_camera_yaw(float angle)
{
	camera.yaw += angle;
}

void rotate_camera_pitch(float angle)
{
  camera.pitch += angle;
}

vec3_t get_camera_lookat_target()
{
	// Initialize the target looking at the positive z-axis
	vec3_t target = { 0, 0, 1 };

	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);

	// Create camera rotation matrix based on yaw and pitch
	mat4_t camera_rotation = mat4_identity();
	camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
	camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

	// Update camera direction based on the rotation
	vec4_t camera_direction = mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
	camera.direction = vec3_from_vec4(camera_direction);

	// Offset the camera position in the direction where the camera is pointing at
	target = vec3_add(camera.position, camera.direction);

	return target;
}

void update_camera_position(vec3_t position)
{
	camera.position.x += position.x;
	camera.position.y += position.y;
	camera.position.z += position.z;
}

void update_camera_direction(vec3_t direction)
{
	camera.direction.x += direction.x;
	camera.direction.y += direction.y;
	camera.direction.z += direction.z;
}

void update_camera_forward_velocity(vec3_t forward_velocity)
{
	camera.forward_velocity.x += forward_velocity.x;
	camera.forward_velocity.y += forward_velocity.y;
	camera.forward_velocity.z += forward_velocity.z;
}
