#pragma once

#include "platform.h"
#include <glad/glad.h>

struct Vao
{
    GLuint handle;
    GLuint programHandle;
};

struct VertexBufferAttribute
{
    u8 location;
    u8 componentCount;
    u8 offset;
};

struct VertexBufferLayout
{
    std::vector<VertexBufferAttribute> attributes;
    u8 stride;
};

struct Submesh
{
    VertexBufferLayout vertexBufferLayout;
    std::vector<float> vertices;
    std::vector<u32>   indices;
    u32                vertexOffset;
    u32                indexOffset;

    std::vector<Vao>   vaos;
};

class Mesh
{
public:
    Mesh();
	~Mesh();

    inline const std::vector<Submesh> GetSubMeshes() const { return submeshes; };
    inline Submesh& GetSubMesh(u32 index) { return submeshes[index]; };
    inline const GLuint GetVertexBuffer() const { return vertexBufferHandle; };
    inline const GLuint GetIndexBuffer() const { return indexBufferHandle; };

    inline GLuint* GetVertexBufferRef() { return &vertexBufferHandle; };
    inline GLuint* GetIndexBufferRef() { return &indexBufferHandle; };

    inline void AddSubMesh(Submesh mesh) { submeshes.push_back(mesh); };
private:

    std::vector<Submesh> submeshes;
    GLuint               vertexBufferHandle;
    GLuint               indexBufferHandle;

};

