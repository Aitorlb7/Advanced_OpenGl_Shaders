#pragma once
#include "platform.h"
#include <glad/glad.h>

//struct Transform
//{	
//
//	Transform() {};
//	Transform(glm::mat4 _matrix, vec3 _position) 
//	{
//		matrix = _matrix;
//		position = _position;	
//	}
//
//	
//};

class Entity
{
public:
	Entity();
	Entity(std::string name, mat4 matrix, vec3 position);
	~Entity();

	inline const std::string GetName() const { return name; };
	inline const mat4 GetWorldMatrix() const { return worldMatrix; };
	inline const vec3 GetPosition() const { return position; };
	inline const u32 GetLocalParamsOffset() const { return localParamsOffset; };
	inline const u32 GetLocalParamsSize() const { return localParamsSize; };


	inline void SetLocalParamsOffset(GLuint _localParamsOffset) { localParamsOffset = _localParamsOffset; };
	inline void SetLocalParamsSize(GLuint _localParamsSize) { localParamsSize = _localParamsSize; };


public:

private:
	
	std::string name;

	u32 modelIndex;
	u32 localParamsOffset = 0;
	u32 localParamsSize = 0;

	mat4 worldMatrix;
	vec3 position;
};

