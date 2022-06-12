#include "Entity.h"
#include "engine.h"

Entity::Entity(): 
	entityType(EntityType::MODEL)
{
}

Entity::Entity(std::string name,EntityType type, vec3 position, vec3 size):
	name(name),
	entityType(type),
	worldMatrix(TransformPositionScale(position,size)),
	position(position)
{
}

Entity::~Entity()
{
}