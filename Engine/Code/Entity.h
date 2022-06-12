#pragma once
#include "platform.h"
#include <glad/glad.h>

enum class EntityType
{
	MODEL,
	SPHERE,
	CUBE,
	PLANE
};

class Entity
{
public:
	Entity();
	Entity(std::string name,EntityType type,vec3 position, vec3 size);
	~Entity();

	inline const std::string GetName() const { return name; };
	inline const EntityType GetType() const { return entityType; };

	inline const mat4 GetWorldMatrix() const { return worldMatrix; };
	inline const vec3 GetPosition() const { return position; };
	inline const u32 GetLocalParamsOffset() const { return localParamsOffset; };
	inline const u32 GetLocalParamsSize() const { return localParamsSize; };


	inline void SetLocalParamsOffset(GLuint _localParamsOffset) { localParamsOffset = _localParamsOffset; };
	inline void SetLocalParamsSize(GLuint _localParamsSize) { localParamsSize = _localParamsSize; };


protected:
	std::string name;
	EntityType entityType;

private:
	
	u32 modelIndex;
	u32 localParamsOffset = 0;
	u32 localParamsSize = 0;

	mat4 worldMatrix;
	vec3 position;
};

