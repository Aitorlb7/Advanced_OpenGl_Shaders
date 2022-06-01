#include "Light.h"

Light::Light():
	color(vec3(1.0f,1.0f,1.0f)),
	direction(vec3(1.0f, 1.0f, 1.0f)),
	position(vec3(1.0f, 1.0f, 1.0f))
{
}

Light::Light(LightType type, vec3 color, vec3 direction, vec3 position, mat4 worldMatrix):
	type(type),
	color(color),
	direction(direction),
	position(position),
	worldMatrix(worldMatrix)
{}

Light::~Light()
{}