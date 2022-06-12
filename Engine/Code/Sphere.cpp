#include "engine.h"
#include "Sphere.h"
#include "Mesh.h"



Sphere::Sphere() : Entity()
{
	entityType = EntityType::SPHERE;
}

Sphere::Sphere(std::string name, vec3 position, vec3 size) : Entity(name,EntityType::SPHERE,position, size)
{
}

Sphere::~Sphere()
{
}
