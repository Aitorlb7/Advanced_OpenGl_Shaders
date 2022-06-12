#pragma once

#include "Entity.h"

class Plane : public Entity
{
public:

	Plane();
	Plane(std::string name, vec3 position, vec3 size);
	~Plane();

private:

};
