#include "Plane.h"

Plane::Plane(): Entity()
{
	entityType = EntityType::PLANE;
}

Plane::Plane(std::string name, mat4 matrix, vec3 position) : Entity(name, EntityType::PLANE, matrix,position)
{
}

Plane::~Plane()
{
}

void Plane::CreatePlaneProcedurally()
{
}
