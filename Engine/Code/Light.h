#pragma once

#include "platform.h"

enum LightType
{
    LIGHT_DIRECTIONAL,
    LIGHT_POINT
};

class Light
{
public:

    Light();
    Light(LightType type, vec3 color, vec3 direction, vec3 position, mat4 worldMatrix);
    ~Light();

    inline const vec3 GetColor() const { return color; };
    inline const vec3 GetPosition() const { return position; };
    inline const vec3 GetDirection() const { return direction; };
    inline const LightType GetType() const { return type; };
    inline const mat4 GetWorldMatrix() const { return worldMatrix; };

    inline const u32 GetLocalParamsOffset() const { return localParamsOffset; };
    inline const u32 GetLocalParamsSize() const { return localParamsSize; };

    inline void SetLocalParamsOffset(GLuint _localParamsOffset) { localParamsOffset = _localParamsOffset; };
    inline void SetLocalParamsSize(GLuint _localParamsSize) { localParamsSize = _localParamsSize; };

private:
    
    LightType type;
    vec3 color;
    vec3 direction;
    vec3 position;

    mat4 worldMatrix;

    u32 localParamsOffset = 0;
    u32 localParamsSize = 0;
};
