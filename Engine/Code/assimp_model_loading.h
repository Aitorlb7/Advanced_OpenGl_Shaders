#pragma once
#include "platform.h"

struct App;
class aiScene;
class aiMesh;
class aiNode;
class aiMaterial;
class Mesh;
class Material;

namespace ModelLoader
{
	u32 LoadModel(App* app, const char* filename);

	void ProcessAssimpNode(const aiScene* scene, aiNode* node, Mesh* myMesh, u32 baseMeshMaterialIndex, std::vector<u32>& submeshMaterialIndices);

	void ProcessAssimpMesh(const aiScene* scene, aiMesh* mesh, Mesh* myMesh, u32 baseMeshMaterialIndex, std::vector<u32>& submeshMaterialIndices);

	void ProcessAssimpMaterial(App* app, aiMaterial* material, Material& myMaterial, String directory);
};
