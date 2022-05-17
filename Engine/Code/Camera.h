#pragma once
#include "platform.h"
#include <glad/glad.h>

class Camera
{
public:
	Camera();
	~Camera();

	const 

	inline const float GetAspectRatio() const { return aspectRatio; };
	inline const float GetNearPlane() const { return nearPlane; };
	inline const float GetFarPlane() const { return farPlane; };
	inline const vec3 GetTarget() const { return target; };
	inline const vec3 GetPosition() const { return position; };
	inline const mat4 GetProjection() const { return projectionMatrix; };
	inline const mat4 GetView() const { return viewMatrix; };

	inline void SetPosition(vec3 _position) { position = _position; }
	inline void SetAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; };
	inline void SetViewMatrix(glm::mat4 view) { viewMatrix = view; };
	inline void SetProjectionMatrix(float radians) { projectionMatrix = glm::perspective(radians, aspectRatio, nearPlane, farPlane); };

private:
	vec3 target;
	vec3 position;

	float aspectRatio;
	float nearPlane;
	float farPlane;
	
	mat4 projectionMatrix;
	mat4 viewMatrix;
};

