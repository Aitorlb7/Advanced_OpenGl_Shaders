#pragma once

#include "Entity.h"

class Plane : public Entity
{
public:

	Plane();
	Plane(std::string name, mat4 matrix, vec3 position);
	~Plane();

private:

	void CreatePlaneProcedurally();
};
