
#pragma once

#include "platform.h"
#include <glad/glad.h>
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"


class GameObject;
class Light;
class Entity;


struct Buffer
{
    GLuint handle;
    GLenum type;
    u32 size;
    u32 head;
    void* data;
};

struct VertexShaderAttribute
{
    u8 location;
    u8 componentCount;
};
struct VertexShaderLayout
{
    std::vector<VertexShaderAttribute> attributes;
};


struct Program
{
    GLuint             handle;
    std::string        filepath;
    std::string        programName;
    u64                lastWriteTimestamp;

    VertexShaderLayout vertexInputLayout;
};

enum Mode
{
    TEXTURED_GEOMETRY,
    TEXTURED_MESH,
    NONE
};

struct VertexV3V2
{
    glm::vec3 pos;
    glm::vec2 uv;
};

struct Model
{
    u32 meshIdx;
    std::vector<u32> materialIdx;
};

struct App
{

    // Loop
    f32  deltaTime;
    bool isRunning;

    // Input
    Input input;

    // Graphics
    char gpuName[64];
    char openGlVersion[64];

    ivec2 displaySize;

    u32 globalParamsOffset = 0;
    u32 globalParamsSize = 0;

    std::vector<Light*> lights;
    std::vector<Entity*> entities;
    std::vector<Mesh>     meshes;
    std::vector<Model>    models;
    std::vector<Texture>  textures;
    std::vector<Material> materials;
    std::vector<Program>  programs;

    Camera camera;
    float cameraMoveSpeed = 30.f;

    mat4 worldMatrix;
    mat4 worldViewProjectionMatrix;

    // program indices
    u32 texturedGeometryProgramIdx;
    u32 texturedMeshProgramIdx;
    
    // texture indices
    u32 diceTexIdx;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;
    u32 patrickTexIdx;

    // Mode
    Mode mode;

    //Buffer
    Buffer buffer;

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint embeddedVertices;
    GLuint embeddedElements;

    // Location of the texture uniform in the textured quad shader
    GLuint programUniformTexture;

    GLint maxUniformBufferSize;
    GLint uniformBlockAlignment;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint vao;
};

void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

GLuint GetVAO(Mesh& mesh, u32 submeshIndex, const Program& program);

void LoadQuad(App* app);

void LoadModel(App* app);

mat4 TransformScale(const vec3& scaleFactors);

mat4 TransformPositionScale(const vec3& pos, const vec3& scaleFactor);

void HandleInput(App* app);

u32 LoadTexture2D(App* app, const char* filepath);


