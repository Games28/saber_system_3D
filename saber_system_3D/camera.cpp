#include "camera.h"

//static camera_t camera;



Camera::Camera()
{
	position = vec3_new(0, 0, 0);
	direction = vec3_new(0, 0, 0);
	forward_velocity = vec3_new(0, 0, 0);
	yaw = 0.0f;
	pitch = 0.0f;
}

Camera::~Camera()
{
}

//void Camera::init_camera(vec3_t position, vec3_t direction)
//{
//	position = vec3_new(0, 0, 0);
//	direction = vec3_new(0, 0, 0);
//	forward_velocity = vec3_new(0, 0, 0);
//	yaw = 0.0f;
//	pitch = 0.0f;
//
//}

vec3_t Camera::get_camera_position()
{
	return position;
}

vec3_t Camera::get_camera_direction()
{
	return direction;
}

vec3_t Camera::get_camera_forward_velocity()
{
	return forward_velocity;
}

float Camera::get_camera_yaw()
{
	return yaw;
}

float Camera::get_camera_pitch()
{
	return pitch;
}

void Camera::set_new_camera_position(vec3_t position)
{
	position = position;
}

void Camera::set_new_camera_direction(vec3_t direction)
{
	direction = direction;
}

void Camera::set_new_camera_forward_velocity(vec3_t forward_velocity)
{
	forward_velocity = forward_velocity;
}

void Camera::rotate_camera_yaw(float angle)
{
	yaw += angle;
}

void Camera::rotate_camera_pitch(float angle)
{
  pitch += angle;
}

vec3_t Camera::get_camera_lookat_target()
{
	// Initialize the target looking at the positive z-axis
	vec3_t target = { 0, 0, 1 };

	mat4_t camera_yaw_rotation = mat4_make_rotation_y(yaw);
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(pitch);

	// Create camera rotation matrix based on yaw and pitch
	mat4_t camera_rotation = mat4_identity();
	camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
	camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

	// Update camera direction based on the rotation
	vec4_t camera_direction = mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
	direction = vec3_from_vec4(camera_direction);

	// Offset the camera position in the direction where the camera is pointing at
	target = vec3_add(position, direction);

	return target;
}

void Camera::update_camera_position(vec3_t position)
{
	position.x += position.x;
	position.y += position.y;
	position.z += position.z;
}

void Camera::update_camera_direction(vec3_t direction)
{
	direction.x += direction.x;
	direction.y += direction.y;
	direction.z += direction.z;
}

void Camera::update_camera_forward_velocity(vec3_t forward_velocity)
{
	forward_velocity.x += forward_velocity.x;
	forward_velocity.y += forward_velocity.y;
	forward_velocity.z += forward_velocity.z;
}
