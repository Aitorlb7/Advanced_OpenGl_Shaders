#include "engine.h"
#include "Sphere.h"
#include "Mesh.h"



Sphere::Sphere() : Entity()
{
	entityType = EntityType::SPHERE;
}

Sphere::Sphere(std::string name, mat4 matrix, vec3 position) : Entity(name,EntityType::SPHERE, matrix, position)
{
}

Sphere::~Sphere()
{
}
