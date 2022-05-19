#include "Entity.h"

Entity::Entity(): 
	entityType(EntityType::MODEL)
{
}

Entity::Entity(std::string name,EntityType type, glm::mat4 matrix, vec3 position):
	name(name),
	entityType(type),
	worldMatrix(matrix),
	position(position)
{
}

Entity::~Entity()
{
}