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
    Light(LightType type, vec3 color, vec3 direction, vec3 position);
    ~Light();

    inline const vec3 GetColor() const { return color; };
    inline const vec3 GetPosition() const { return position; };
    inline const vec3 GetDirection() const { return direction; };
    inline const LightType GetType() const { return type; };

private:
    
    LightType type;
    vec3 color;
    vec3 direction;
    vec3 position;
};
