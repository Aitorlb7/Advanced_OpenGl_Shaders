#include "Plane.h"
#include "engine.h"

Plane::Plane(): Entity()
{
	entityType = EntityType::PLANE;
}

Plane::Plane(std::string name, vec3 position, vec3 size) : Entity(name, EntityType::PLANE, position, size)
{
}

Plane::~Plane()
{
}
