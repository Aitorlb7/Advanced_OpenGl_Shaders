#pragma once

#include "Entity.h"



class Sphere : public Entity
{
public:

	Sphere();
	Sphere(std::string name, vec3 position, vec3 size);
	~Sphere();

};
