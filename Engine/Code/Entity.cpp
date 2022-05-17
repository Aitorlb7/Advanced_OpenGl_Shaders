#include "Entity.h"

Entity::Entity()
{
}

Entity::Entity(std::string name, glm::mat4 matrix, vec3 position):
	name(name),
	worldMatrix(matrix),
	position(position)
{
}

Entity::~Entity()
{
}