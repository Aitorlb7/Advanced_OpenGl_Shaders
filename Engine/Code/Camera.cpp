#include "Camera.h"

Camera::Camera():
	position(vec3(-10.0f, 15.0f, 45.0f)),
	target(vec3(0.0f, 0.0f, 0.0f)),
	aspectRatio(0.0f),
	nearPlane(0.1f),
	farPlane(1000.0f),
	projectionMatrix(mat4(0.0f)),
	viewMatrix(lookAt(position, target, vec3(0.f, 1.f, 0.f)))
{
}

Camera::~Camera()
{
}