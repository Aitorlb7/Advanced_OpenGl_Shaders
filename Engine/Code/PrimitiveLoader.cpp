#include "PrimitiveLoader.h"
#include "engine.h"
#include "Sphere.h"
#include "Mesh.h"

#define H 32
#define V 32
#define PI 3.1416f

u32 PrimitiveLoader::LoadSphere(App* app)
{
	std::vector<float> vertices;
	std::vector<u32> indices;

	PrimitiveVertex sphereVertex[H][V + 1];

	for (int h = 0; h < H; ++h)
	{
		for (int v = 0; v < V + 1; ++v)
		{
			float nh = float(h) / H;
			float nv = float(v) / V - 0.5f;
			float angleh = 2 * PI * nh;
			float anglev = -PI * nv;
			sphereVertex[h][v].pos.x = sinf(angleh) * cosf(anglev);
			sphereVertex[h][v].pos.y = -sinf(anglev);
			sphereVertex[h][v].pos.z = cosf(angleh) * cosf(anglev);
			sphereVertex[h][v].normals = sphereVertex[h][v].pos;

			vertices.push_back(sphereVertex[h][v].pos.x);
			vertices.push_back(sphereVertex[h][v].pos.y);
			vertices.push_back(sphereVertex[h][v].pos.z);
			vertices.push_back(sphereVertex[h][v].normals.x);
			vertices.push_back(sphereVertex[h][v].normals.y);
			vertices.push_back(sphereVertex[h][v].normals.z);

			vertices.push_back(0.0f);
			vertices.push_back(0.0f);

			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);

			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);

		}
	}

	u32 sphereIndices[H][V][6];

	for (u32 h = 0; h < H; ++h)
	{
		for (u32 v = 0; v < V; ++v)
		{
			sphereIndices[h][v][0] = (h + 0) * (V + 1) + v;
			sphereIndices[h][v][1] = ((h + 1) % H) * (V + 1) + v;
			sphereIndices[h][v][2] = ((h + 1) % H) * (V + 1) + v + 1;
			sphereIndices[h][v][3] = (h + 0) * (V + 1) + v;
			sphereIndices[h][v][4] = ((h + 1) % H) * (V + 1) + v + 1;
			sphereIndices[h][v][5] = (h + 0) * (V + 1) + v + 1;

			indices.push_back(sphereIndices[h][v][0]);
			indices.push_back(sphereIndices[h][v][1]);
			indices.push_back(sphereIndices[h][v][2]);
			indices.push_back(sphereIndices[h][v][3]);
			indices.push_back(sphereIndices[h][v][4]);
			indices.push_back(sphereIndices[h][v][5]);
		}
	}



	app->meshes.push_back(Mesh{});
	Mesh& mesh = app->meshes.back();
	u32 meshIdx = (u32)app->meshes.size() - 1u;

	app->models.push_back(Model{});
	Model& model = app->models.back();
	model.meshIdx = meshIdx;
	u32 modelIdx = (u32)app->models.size() - 1u;


	u32 baseMeshMaterialIndex = (u32)app->materials.size();
	app->materials.push_back(Material{});
	Material& material = app->materials.back();

	material.SetName("SphereMaterial");

	material.SetAlbedoTextureIdx(app->whiteTexIdx);

	model.materialIdx.push_back(baseMeshMaterialIndex);


	VertexBufferLayout vertexBufferLayout = {};

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
	vertexBufferLayout.stride = 6 * sizeof(float);

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, vertexBufferLayout.stride });
	vertexBufferLayout.stride += 2 * sizeof(float);

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 3, 3, vertexBufferLayout.stride });
	vertexBufferLayout.stride += 3 * sizeof(float);

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 4, 3, vertexBufferLayout.stride });
	vertexBufferLayout.stride += 3 * sizeof(float);


	Submesh submesh = {};
	submesh.vertexBufferLayout = vertexBufferLayout;
	submesh.vertices.swap(vertices);
	submesh.indices.swap(indices);
	mesh.AddSubMesh(submesh);

	u32 vertexBufferSize = 0;
	u32 indexBufferSize = 0;

	for (u32 i = 0; i < mesh.GetSubMeshes().size(); ++i)
	{
		vertexBufferSize += mesh.GetSubMesh(i).vertices.size() * sizeof(float);
		indexBufferSize += mesh.GetSubMesh(i).indices.size() * sizeof(u32);
	}

	glGenBuffers(1, mesh.GetVertexBufferRef());
	glBindBuffer(GL_ARRAY_BUFFER, mesh.GetVertexBuffer());
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

	glGenBuffers(1, mesh.GetIndexBufferRef());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBuffer());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);

	u32 indicesOffset = 0;
	u32 verticesOffset = 0;

	for (u32 i = 0; i < mesh.GetSubMeshes().size(); ++i)
	{
		const void* verticesData = mesh.GetSubMesh(i).vertices.data();
		const u32   verticesSize = mesh.GetSubMesh(i).vertices.size() * sizeof(float);
		glBufferSubData(GL_ARRAY_BUFFER, verticesOffset, verticesSize, verticesData);
		mesh.GetSubMesh(i).vertexOffset = verticesOffset;
		verticesOffset += verticesSize;

		const void* indicesData = mesh.GetSubMesh(i).indices.data();
		const u32   indicesSize = mesh.GetSubMesh(i).indices.size() * sizeof(u32);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indicesData);
		mesh.GetSubMesh(i).indexOffset = indicesOffset;
		indicesOffset += indicesSize;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	return modelIdx;
}

u32 PrimitiveLoader::LoadPlane(App* app)
{

	app->meshes.push_back(Mesh{});
	Mesh& mesh = app->meshes.back();
	u32 meshIdx = (u32)app->meshes.size() - 1u;

	app->models.push_back(Model{});
	Model& model = app->models.back();
	model.meshIdx = meshIdx;
	u32 modelIdx = (u32)app->models.size() - 1u;


	u32 baseMeshMaterialIndex = (u32)app->materials.size();
	app->materials.push_back(Material{});
	Material& material = app->materials.back();

	material.SetName("PlaneMaterial");

	material.SetAlbedoTextureIdx(LoadTexture2D(app, "Wall/WallDiffuse.png"));
	material.SetNormalsTextureIdx(LoadTexture2D(app, "Wall/WallNormal.png"));
	material.SetBumpTextureIdx(LoadTexture2D(app, "Wall/WallDisplacement.png"));

	model.materialIdx.push_back(baseMeshMaterialIndex);


	VertexBufferLayout vertexBufferLayout = {};

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 0, 3, 0 });
	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 1, 3, 3 * sizeof(float) });
	vertexBufferLayout.stride = 6 * sizeof(float);

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 2, 2, vertexBufferLayout.stride });
	vertexBufferLayout.stride += 2 * sizeof(float);

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 3, 3, vertexBufferLayout.stride });
	vertexBufferLayout.stride += 3 * sizeof(float);

	vertexBufferLayout.attributes.push_back(VertexBufferAttribute{ 4, 3, vertexBufferLayout.stride });
	vertexBufferLayout.stride += 3 * sizeof(float);


	Submesh submesh = {};
	submesh.vertexBufferLayout = vertexBufferLayout;

	submesh.vertices.insert(submesh.vertices.end(), { -1,1,0,  0,0,1,  0,1, 1,0,0, 0,1,0, });
	submesh.vertices.insert(submesh.vertices.end(), { -1,-1,0, 0,0,1,  0,0, 1,0,0, 0,1,0, });
	submesh.vertices.insert(submesh.vertices.end(), { 1,-1,0,  0,0,1,  1,0, 1,0,0, 0,1,0, });
	submesh.vertices.insert(submesh.vertices.end(), { 1,1,0,   0,0,1,  1,1, 1,0,0, 0,1,0, });

	submesh.indices.insert(submesh.indices.end(), { 0,1,2,0,2,3 });
	
	mesh.AddSubMesh(submesh);

	u32 vertexBufferSize = 0;
	u32 indexBufferSize = 0;

	for (u32 i = 0; i < mesh.GetSubMeshes().size(); ++i)
	{
		vertexBufferSize += mesh.GetSubMesh(i).vertices.size() * sizeof(float);
		indexBufferSize += mesh.GetSubMesh(i).indices.size() * sizeof(u32);
	}

	glGenBuffers(1, mesh.GetVertexBufferRef());
	glBindBuffer(GL_ARRAY_BUFFER, mesh.GetVertexBuffer());
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

	glGenBuffers(1, mesh.GetIndexBufferRef());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBuffer());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);

	u32 indicesOffset = 0;
	u32 verticesOffset = 0;

	for (u32 i = 0; i < mesh.GetSubMeshes().size(); ++i)
	{
		const void* verticesData = mesh.GetSubMesh(i).vertices.data();
		const u32   verticesSize = mesh.GetSubMesh(i).vertices.size() * sizeof(float);
		glBufferSubData(GL_ARRAY_BUFFER, verticesOffset, verticesSize, verticesData);
		mesh.GetSubMesh(i).vertexOffset = verticesOffset;
		verticesOffset += verticesSize;

		const void* indicesData = mesh.GetSubMesh(i).indices.data();
		const u32   indicesSize = mesh.GetSubMesh(i).indices.size() * sizeof(u32);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indicesData);
		mesh.GetSubMesh(i).indexOffset = indicesOffset;
		indicesOffset += indicesSize;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	return modelIdx;
}
