
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

enum RenderTarget {
    FINAL,
    POSITION,
    NORMAL,
    DIFFUSE,
    DEPTH,
    SKYBOX
};


struct PrimitiveVertex { vec3 pos; vec3 normals; };

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
    bool reflectionsActive = false;
    bool useNormalMapping = false;
    bool useBumpMapping = false;
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
    u32 directionalLightProgramIdx;
    u32 pointLightProgramIdx;
    u32 skyboxProgramIdx;
    u32 reflectionProgramIdx;
    u32 normalMapProgramIdx;
    u32 bumpMapProgramIdx;

    // texture indices
    u32 diceTexIdx;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;

    u32 skyboxTexId;

    u32 modelPatrickId;
    u32 modelSphereId;
    u32 modelPlaneId;

    // Mode
    Mode mode;

    //Buffer
    Buffer buffer;

    //Framebuffer
    GLuint framebufferVAO;
    GLuint framebufferHandle;
    GLuint depthAttachmentHandle;
    GLuint colorAttachmentHandle;
    u32 drawFramebufferProgramIdx;

    GLuint Gbuffer;
    GLuint GAlbedo;
    GLuint GPosition;
    GLuint GNormal;
    GLuint GDepth;
    GLuint GSkybox;
    GLuint GFinal;

    RenderTarget renderTarget;
    u32 renderAttachment;
    const char* renderTargetsChar[6] = {"Final", "Position", "Normals", "Albedo", "Depth" , "Skybox"};

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint embeddedVertices;
    GLuint embeddedElements;

    // Location of the texture uniform in the textured quad shader
    GLuint  programUniformTexture;
    GLuint  programUniformSkyboxTexture;

    GLuint  pointUniformPosition;
    GLuint  pointUniformNormal;
    GLuint  pointUniformDiffuse;
    GLuint  pointUniformDepth;

    GLuint  directionalUniformPosition;
    GLuint  directionalUniformNormal;
    GLuint  directionalUniformDiffuse;
    GLuint  directionalUniformDepth;

    GLint maxUniformBufferSize;
    GLint uniformBlockAlignment;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint vao;
};
void InitGBuffers(App* app);

void SetUpDeferredShading(App* app);

void Init(App* app);

void Gui(App* app);

void Update(App* app);

void Render(App* app);

GLuint GetVAO(Mesh& mesh, u32 submeshIndex, const Program& program);

void LoadQuad(App* app);

void LoadAtributes(App* app, Program* program);

mat4 TransformScale(const vec3& scaleFactors);

mat4 TransformPositionScale(const vec3& pos, const vec3& scaleFactor);

void HandleInput(App* app);

u32 LoadTexture2D(App* app, const char* filepath);



