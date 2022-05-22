#pragma once

#include "Entity.h"



class Sphere : public Entity
{
public:

	Sphere();
	Sphere(std::string name, mat4 matrix, vec3 position);
	~Sphere();

};
