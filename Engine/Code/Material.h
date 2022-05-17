#pragma once

#include "platform.h"

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};

struct Texture
{
    GLuint      handle;
    std::string filepath;
};

class Material
{
public:
    Material();
	~Material();

    inline const u32 GetAlbedoTextureIdx() const { return albedoTextureIdx; };

    inline void SetName(std::string _name) { name = _name; };
    inline void SetAlbedo(vec3 _albedo) { albedo = _albedo; };
    inline void SetEmissive(vec3 _emissive) { emissive = _emissive; };
    inline void SetSmoothness(f32 _smoothness) { smoothness = _smoothness; };
    inline void SetAlbedoTextureIdx(u32 _albedoTextureIdx) { albedoTextureIdx = _albedoTextureIdx; };
    inline void SetEmissiveTextureIdx(u32 _emissiveTextureIdx) { emissiveTextureIdx = _emissiveTextureIdx; };
    inline void SetSpecularTextureIdx(u32 _specularTextureIdx) { specularTextureIdx = _specularTextureIdx; };
    inline void SetNormalsTextureIdx(u32 _normalsTextureIdx) { normalsTextureIdx = _normalsTextureIdx; };
    inline void SetBumpTextureIdx(u32 _bumpTextureIdx) { bumpTextureIdx = _bumpTextureIdx; };


private:

    std::string name;
    vec3        albedo;
    vec3        emissive;
    f32         smoothness;
    u32         albedoTextureIdx;
    u32         emissiveTextureIdx;
    u32         specularTextureIdx;
    u32         normalsTextureIdx;
    u32         bumpTextureIdx;

};

