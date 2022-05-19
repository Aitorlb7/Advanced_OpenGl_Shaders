#pragma once

#include "Entity.h"

struct SphereVertex { vec3 pos; vec3 normals; };

class Sphere : public Entity
{
public:

	Sphere();
	Sphere(std::string name, mat4 matrix, vec3 position);
	~Sphere();

	u32 LoadSphere(App* app);

private:

	const float pi;
};
