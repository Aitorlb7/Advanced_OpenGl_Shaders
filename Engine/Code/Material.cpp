#include "Material.h"

Material::Material() :
	name(""),
	albedo(vec3(0.0f,0.0f,0.0f)),
	emissive(vec3(0.0f, 0.0f, 0.0f)),
	smoothness(0.0f),
	albedoTextureIdx(0),
	emissiveTextureIdx(0),
	specularTextureIdx(0),
	normalsTextureIdx(0),
	bumpTextureIdx(0)
{
}

Material::~Material()
{
}