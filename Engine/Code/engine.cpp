
#include "engine.h"
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include "AssimpLoader.h"
#include "PrimitiveLoader.h"
#include "BufferManager.h"
#include "Entity.h"
#include "Sphere.h"
#include "Plane.h"
#include "Light.h"

GLuint CreateProgramFromSource(String programSource, const char* shaderName)
{
    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 430\n";
    char shaderNameDefine[128];
    sprintf(shaderNameDefine, "#define %s\n", shaderName);
    char vertexShaderDefine[] = "#define VERTEX\n";
    char fragmentShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        shaderNameDefine,
        vertexShaderDefine,
        programSource.str
    };
    const GLint vertexShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(vertexShaderDefine),
        (GLint) programSource.len
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        shaderNameDefine,
        fragmentShaderDefine,
        programSource.str
    };
    const GLint fragmentShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(fragmentShaderDefine),
        (GLint) programSource.len
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vshader);
    glAttachShader(programHandle, fshader);
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    glUseProgram(0);

    glDetachShader(programHandle, vshader);
    glDetachShader(programHandle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programHandle;
}

u32 LoadProgram(App* app, const char* filepath, const char* programName)
{
    String programSource = ReadTextFile(filepath);

    Program program = {};
    program.handle = CreateProgramFromSource(programSource, programName);
    program.filepath = filepath;
    program.programName = programName;
    program.lastWriteTimestamp = GetFileLastWriteTimestamp(filepath);
    app->programs.push_back(program);

    return app->programs.size() - 1;
}

Image LoadImage(const char* filename)
{
    Image img = {};
    stbi_set_flip_vertically_on_load(true);
    img.pixels = stbi_load(filename, &img.size.x, &img.size.y, &img.nchannels, 0);
    if (img.pixels)
    {
        img.stride = img.size.x * img.nchannels;
    }
    else
    {
        ELOG("Could not open file %s", filename);
    }
    return img;
}

void FreeImage(Image image)
{
    stbi_image_free(image.pixels);
}

GLuint CreateTexture2DFromImage(Image image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat     = GL_RGB;
    GLenum dataType       = GL_UNSIGNED_BYTE;

    switch (image.nchannels)
    {
        case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
        case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
        default: ELOG("LoadTexture2D() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texHandle;
}

u32 LoadTexture2D(App* app, const char* filepath)
{
    for (u32 texIdx = 0; texIdx < app->textures.size(); ++texIdx)
        if (app->textures[texIdx].filepath == filepath)
            return texIdx;

    Image image = LoadImage(filepath);

    if (image.pixels)
    {
        Texture tex = {};
        tex.handle = CreateTexture2DFromImage(image);
        tex.filepath = filepath;

        u32 texIdx = app->textures.size();
        app->textures.push_back(tex);

        FreeImage(image);
        return texIdx;
    }
    else
    {
        return UINT32_MAX;
    }
}

bool InitGBuffers(App* app)
{


        //Create QUAD to render
    VertexV3V2 vertices[] = {
        {glm::vec3(-1.0,-1.0,0.0),glm::vec2(0.0,0.0)},
        {glm::vec3(1.0,-1.0,0.0),glm::vec2(1.0,0.0) },
        {glm::vec3(1.0,1.0,0.0),glm::vec2(1.0,1.0) },
        {glm::vec3(-1.0,1.0,0.0),glm::vec2(0.0,1.0) },
    };

    u16 indices[] = {
        0,1,2,
        0,2,3
    };

    glGenBuffers(1, &app->embeddedVertices);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &app->embeddedElements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    // Create the FBO
    glGenFramebuffers(1, &app->Gbuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, app->Gbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBindVertexArray(0);

    app->drawFramebufferProgramIdx = LoadProgram(app, "shaders.glsl", "QUAD");
    Program& program = app->programs[app->drawFramebufferProgramIdx];
    app->programUniformTexture = glGetUniformLocation(program.handle, "textureSampler");

    if (app->programUniformTexture == GL_INVALID_VALUE || app->programUniformTexture == GL_INVALID_OPERATION)
    {
        ILOG("Framebuffer program loading error");
    }

        // depth
    glBindTexture(GL_TEXTURE_2D, app->depthAttachmentHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, app->displaySize.x, app->displaySize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
        NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, app->depthAttachmentHandle, 0);

    // - position color buffer
    glGenTextures(1, &app->GPosition);
    glBindTexture(GL_TEXTURE_2D, app->GPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, app->GPosition, 0);

    // - normal color buffer
    glGenTextures(1, &app->GNormal);
    glBindTexture(GL_TEXTURE_2D, app->GNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, app->GNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &app->GAlbedo);
    glBindTexture(GL_TEXTURE_2D, app->GAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, app->GAlbedo, 0);

    // - color + specular color buffer
    glGenTextures(1, &app->GDepth);
    glBindTexture(GL_TEXTURE_2D, app->GDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, app->GDepth, 0);


    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (framebufferStatus)
        {
        case GL_FRAMEBUFFER_UNDEFINED: ELOG("GL_FRAMEBUFFER_UNDEFINED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: ELOG("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: ELOG("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: ELOG("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: ELOG("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
        case GL_FRAMEBUFFER_UNSUPPORTED: ELOG("GL_FRAMEBUFFER_UNSUPPORTED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: ELOG("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: ELOG("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
        default: ELOG("Unknown framebuffer status error");
        }
    }


    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(ARRAY_COUNT(DrawBuffers), DrawBuffers);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}


void InitFramebuffer(App* app)
{
    //Create QUAD to render
    VertexV3V2 vertices[] = {
        {glm::vec3(-1.0,-1.0,0.0),glm::vec2(0.0,0.0)},
        {glm::vec3(1.0,-1.0,0.0),glm::vec2(1.0,0.0) },
        {glm::vec3(1.0,1.0,0.0),glm::vec2(1.0,1.0) },
        {glm::vec3(-1.0,1.0,0.0),glm::vec2(0.0,1.0) },
    };

    u16 indices[] = {
        0,1,2,
        0,2,3
    };

    glGenBuffers(1, &app->embeddedVertices);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &app->embeddedElements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    glGenVertexArrays(1, &app->framebufferVAO);
    glBindVertexArray(app->framebufferVAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBindVertexArray(0);

    app->drawFramebufferProgramIdx = LoadProgram(app, "shaders.glsl", "QUAD");
    Program& program = app->programs[app->drawFramebufferProgramIdx];
    app->programUniformTexture = glGetUniformLocation(program.handle, "textureSampler");

    if (app->programUniformTexture == GL_INVALID_VALUE || app->programUniformTexture == GL_INVALID_OPERATION)
    {
        ILOG("Framebuffer program loading error");
    }

    //Set up framebuffers

    app->colorAttachmentHandle;
    glGenTextures(1, &app->colorAttachmentHandle);
    glBindTexture(GL_TEXTURE_2D, app->colorAttachmentHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, app->displaySize.x, app->displaySize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    app->depthAttachmentHandle;
    glGenTextures(1, &app->depthAttachmentHandle);
    glBindTexture(GL_TEXTURE_2D, app->depthAttachmentHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, app->displaySize.x, app->displaySize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    app->framebufferHandle;
    glGenFramebuffers(1, &app->framebufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, app->framebufferHandle);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, app->colorAttachmentHandle, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, app->depthAttachmentHandle, 0);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (framebufferStatus)
        {
        case GL_FRAMEBUFFER_UNDEFINED: ELOG("GL_FRAMEBUFFER_UNDEFINED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: ELOG("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: ELOG("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: ELOG("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: ELOG("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
        case GL_FRAMEBUFFER_UNSUPPORTED: ELOG("GL_FRAMEBUFFER_UNSUPPORTED"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: ELOG("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: ELOG("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
        default: ELOG("Unknown framebuffer status error");
        }
    }

    glDrawBuffers(1, &app->colorAttachmentHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Init(App* app)
{
    glEnable(GL_DEPTH_TEST);

    app->mode = Mode::TEXTURED_MESH;


    app->diceTexIdx = LoadTexture2D(app, "dice.png");
    app->whiteTexIdx = LoadTexture2D(app, "color_white.png");
    app->blackTexIdx = LoadTexture2D(app, "color_black.png");
    app->normalTexIdx = LoadTexture2D(app, "color_normal.png");
    app->magentaTexIdx = LoadTexture2D(app, "color_magenta.png");


    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &app->maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->uniformBlockAlignment);


    //TODO max size buffers

    app->camera.SetAspectRatio((float)app->displaySize.x / (float)app->displaySize.y);

    app->camera.SetProjectionMatrix(glm::radians(60.0f));

    app->worldMatrix = TransformPositionScale(vec3(0.f, 1.f, 0.f), vec3(1.f)); //arbitrary position of the model, later should take th entitie's position
    app->worldViewProjectionMatrix = app->camera.GetProjection() * app->camera.GetView() * app->worldMatrix;    


    app->entities.push_back(new Entity("Patrick1", EntityType::MODEL, TransformPositionScale(vec3(5.f, 0.f, -5.f), vec3(1.f)), vec3(5.f, 0.f, -5.f)));
    app->entities.push_back(new Entity("Patrick2", EntityType::MODEL, TransformPositionScale(vec3(0.f, 0.f, 0.f), vec3(1.f)), vec3(0.f, 0.f, 0.f)));
    app->entities.push_back(new Entity("Patrick3", EntityType::MODEL, TransformPositionScale(vec3(-5.f, 0.f, -5.f), vec3(1.f)), vec3(-5.f, 0.f, -5.f)));
    app->entities.push_back(new Sphere("Sphere", TransformPositionScale(vec3(0.f, 5.f, 0.f), vec3(1.f)), vec3(0.f, 5.f, 0.f)));
    app->entities.push_back(new Plane("Plane", TransformPositionScale(vec3(0.f, -3.5f, 0.f), vec3(15.f)), vec3(0.f, -5.f, 0.f)));

    
    app->modelPatrickId = ModelLoader::LoadModel(app, "Patrick/Patrick.obj"); 
    app->modelSphereId = PrimitiveLoader::LoadSphere(app);
    app->modelPlaneId = PrimitiveLoader::LoadPlane(app);



    app->lights.push_back(new Light(LIGHT_DIRECTIONAL, vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f)));
    app->lights.push_back(new Light(LIGHT_POINT, vec3(0.f, 0.f, 0.7f), vec3(0.0f, 0.0f, .0f), vec3(0.0f, 10.0f, -2.0f)));

    //InitFramebuffer(app);

    InitGBuffers(app);

    switch (app->mode)
    {
        case TEXTURED_GEOMETRY: LoadQuad(app);  break;
        case TEXTURED_MESH: LoadModel(app); break;
        case NONE: break;
    }


}

void Gui(App* app)
{
    ImGui::Begin("Info");
    ImGui::Text("FPS: %f", 1.0f/app->deltaTime);

    if (ImGui::Begin("Camera"))
    {
        vec3 position = app->camera.GetPosition();

        if (ImGui::DragFloat("X", (float*)&position.x, 0.1))
        {
            app->camera.SetPosition(position);

        };
        if (ImGui::DragFloat("Y", (float*)&position.y, 0.1))
        {
            app->camera.SetPosition(position);

        };
        if (ImGui::DragFloat("Z", (float*)&position.z, 0.1))
        {
            app->camera.SetPosition(position);

        };



        ImGui::End();
    }

    ImGui::End();
}

void Update(App* app)
{
    HandleInput(app);
    //app->camera.SetViewMatrix(lookAt(app->camera.GetPosition(), app->camera.GetTarget(), vec3(0.f, 1.f, 0.f)));
    app->camera.SetViewMatrix(translate(app->camera.GetPosition()));

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, app->Gbuffer);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




    MapBuffer(app->buffer, GL_WRITE_ONLY);

    //Initialize global uniforms
    app->globalParamsOffset = app->buffer.head;

    PushVec3(app->buffer, app->camera.GetPosition());

    PushUInt(app->buffer, app->lights.size());

    for (u32 i = 0; i < app->lights.size(); ++i)
    {
        AlignHead(app->buffer, sizeof(vec4));

        Light* light = app->lights[i];
        PushUInt(app->buffer, light->GetType());
        PushVec3(app->buffer, light->GetColor());
        PushVec3(app->buffer, light->GetDirection());
        PushVec3(app->buffer, light->GetPosition());
    }

    app->globalParamsSize = app->buffer.head - app->globalParamsOffset;


    for (int i = 0; i < app->entities.size(); ++i)
    {
        AlignHead(app->buffer, app->uniformBlockAlignment);

        app->entities[i]->SetLocalParamsOffset(app->buffer.head);

        PushMat4(app->buffer, app->entities[i]->GetWorldMatrix());
        PushMat4(app->buffer, app->camera.GetProjection() * app->camera.GetView() * app->entities[i]->GetWorldMatrix());

        app->entities[i]->SetLocalParamsSize(app->buffer.head - app->entities[i]->GetLocalParamsOffset());
    }

    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}


void GeometryPass(App* app)
{
    //switch (app->mode)
    //{
    //    case TEXTURED_GEOMETRY:
    //    {
    //        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    //        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //        glViewport(0, 0, app->displaySize.x, app->displaySize.y);

    //        Program& geometryProgram = app->programs[app->texturedGeometryProgramIdx];
    //        glUseProgram(geometryProgram.handle);
    //        glBindVertexArray(app->vao);

    //        glEnable(GL_BLEND);
    //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //        glUniform1i(app->programUniformTexture, 0);
    //        glActiveTexture(GL_TEXTURE0);
    //        GLuint textureHandle = app->textures[app->diceTexIdx].handle;
    //        glBindTexture(GL_TEXTURE_2D, textureHandle);

    //        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    //        glBindVertexArray(0);
    //        glUseProgram(0);
    //        break;
    //    }
    //    case TEXTURED_MESH:
    //    {
    //        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //        //Bind buffer range with binding 0 for global params (light)
    //        glBindBufferRange(GL_UNIFORM_BUFFER, 0, app->buffer.handle, app->globalParamsOffset, app->globalParamsSize);

    //        for (int i = 0; i < app->entities.size(); ++i)
    //        {
    //            glBindBufferRange(GL_UNIFORM_BUFFER, 1, app->buffer.handle, app->entities[i]->GetLocalParamsOffset(), sizeof(mat4) * 2);

    //            Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];
    //            glUseProgram(texturedMeshProgram.handle);

    //            Model model;

    //            switch (app->entities[i]->GetType())
    //            {
    //            case EntityType::MODEL: model = app->models[app->modelPatrickId]; break;
    //            case EntityType::SPHERE: model = app->models[app->modelSphereId]; break;
    //            case EntityType::PLANE: model = app->models[app->modelPlaneId]; break;

    //            }

    //            
    //            Mesh& mesh = app->meshes[model.meshIdx];

    //            for (u32 i = 0; i < mesh.GetSubMeshes().size(); ++i)
    //            {
    //                GLuint vao = GetVAO(mesh, i, texturedMeshProgram);
    //                glBindVertexArray(vao);

    //                u32 submeshMaterialIdx = model.materialIdx[i];
    //                Material& submeshMaterial = app->materials[submeshMaterialIdx];

    //                glActiveTexture(GL_TEXTURE0);
    //                glBindTexture(GL_TEXTURE_2D, app->textures[submeshMaterial.GetAlbedoTextureIdx()].handle);
    //                glUniform1i(0, 0); //app->texturedMeshProgram_uTexture?????

    //                Submesh& submesh = mesh.GetSubMesh(i);
    //                glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(u64)submesh.indexOffset);

    //            }


    //        }
    //        break;
    //    }

    //    default:;
    //}

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, app->buffer.handle, app->globalParamsOffset, app->globalParamsSize);

    Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];


    for (int i = 0; i < app->entities.size(); ++i)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, app->buffer.handle, app->entities[i]->GetLocalParamsOffset(), sizeof(mat4) * 2);

        glUseProgram(texturedMeshProgram.handle);

        Model model;

        switch (app->entities[i]->GetType())
        {
        case EntityType::MODEL: model = app->models[app->modelPatrickId]; break;
        case EntityType::SPHERE: model = app->models[app->modelSphereId]; break;
        case EntityType::PLANE: model = app->models[app->modelPlaneId]; break;

        }


        Mesh& mesh = app->meshes[model.meshIdx];

        for (u32 i = 0; i < mesh.GetSubMeshes().size(); ++i)
        {
            GLuint vao = GetVAO(mesh, i, texturedMeshProgram);
            glBindVertexArray(vao);

            u32 submeshMaterialIdx = model.materialIdx[i];
            Material& submeshMaterial = app->materials[submeshMaterialIdx];

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, app->textures[submeshMaterial.GetAlbedoTextureIdx()].handle);
            glUniform1i(0, 0); //app->texturedMeshProgram_uTexture?????

            Submesh& submesh = mesh.GetSubMesh(i);
            glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, (void*)(u64)submesh.indexOffset);

        }


    }

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void LightPass(App* app)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindFramebuffer(GL_READ_FRAMEBUFFER, app->Gbuffer);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->GPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, app->GNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, app->GAlbedo);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, app->GDepth);
}


void Render(App* app)
{
    

    GeometryPass(app);

    LightPass(app);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //After the meshes rendering
    //Render Framebuffers to screen

    glViewport(0, 0, app->displaySize.x, app->displaySize.y);

    Program& geometryProgram = app->programs[app->drawFramebufferProgramIdx];
    glUseProgram(geometryProgram.handle);
    glBindVertexArray(app->Gbuffer);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform1i(app->programUniformTexture, 0);
    glActiveTexture(GL_TEXTURE2);

    glBindTexture(GL_TEXTURE_2D, app->GAlbedo);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

}

GLuint GetVAO(Mesh& mesh, u32 submeshIndex, const Program& program)
{
    Submesh& submesh = mesh.GetSubMesh(submeshIndex);

    //Try finding a vao for this submesh/program
    for (u32 i = 0; i < (u32)submesh.vaos.size(); ++i)
        if (submesh.vaos[i].programHandle == program.handle)
            return submesh.vaos[i].handle;

    GLuint vaoHandle = 0;

    //Create a new vao for this submesh program
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.GetVertexBuffer());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndexBuffer());

    // We have to link all vertex inputs attributes to attributes in the vertex buffer
    for (u32 i = 0; i < program.vertexInputLayout.attributes.size(); ++i)
    {
        bool attributeWasLinked = false;

        for (u32 j = 0; j < submesh.vertexBufferLayout.attributes.size(); ++j)
        {
            if (program.vertexInputLayout.attributes[i].location == submesh.vertexBufferLayout.attributes[j].location)
            {
                const u32 location = submesh.vertexBufferLayout.attributes[j].location;

                glVertexAttribPointer(location,
                    submesh.vertexBufferLayout.attributes[j].componentCount, 
                    GL_FLOAT, GL_FALSE, submesh.vertexBufferLayout.stride, 
                    (void*)(u64)(submesh.vertexBufferLayout.attributes[j].offset + submesh.vertexOffset));

                glEnableVertexAttribArray(location);

                attributeWasLinked = true;
                break;
            }
        }
        assert(attributeWasLinked); //The submesh should provide an attribute for each vertex inputs
    }

    glBindVertexArray(0);

    //Store it in the list of vaos for this submesh
    Vao vao{ vaoHandle,program.handle };
    submesh.vaos.push_back(vao);

    return vaoHandle;
}

void LoadQuad(App* app)
{
    VertexV3V2 vertices[] = {
    {glm::vec3(-1.0,-1.0,0.0),glm::vec2(0.0,0.0)},
    {glm::vec3(1.0,-1.0,0.0),glm::vec2(1.0,0.0) },
    {glm::vec3(1.0,1.0,0.0),glm::vec2(1.0,1.0) },
    {glm::vec3(-1.0,1.0,0.0),glm::vec2(0.0,1.0) },
    };

    u16 indices[] = {
        0,1,2,
        0,2,3
    };

    //Geometry buffers

    glGenBuffers(1, &app->embeddedVertices);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &app->embeddedElements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //

    glGenVertexArrays(1, &app->vao);
    glBindVertexArray(app->vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->embeddedVertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->embeddedElements);
    glBindVertexArray(0);

    app->texturedGeometryProgramIdx = LoadProgram(app, "shaders.glsl", "TEXTURED_GEOMETRY");
    Program& program = app->programs[app->texturedGeometryProgramIdx];
    app->programUniformTexture = glGetUniformLocation(program.handle, "textureSampler");


    app->diceTexIdx = LoadTexture2D(app, "dice.png");
}

void LoadModel(App* app)
{
    app->texturedMeshProgramIdx = LoadProgram(app, "shaders.glsl", "TEXTURED_MESH");
    Program& texturedMeshProgram = app->programs[app->texturedMeshProgramIdx];

    int attributeCount = 0;
    glGetProgramiv(texturedMeshProgram.handle, GL_ACTIVE_ATTRIBUTES, &attributeCount);

    for (int i = 0; i < attributeCount; ++i)
    {
        GLchar attribName[100];
        int attribNameLength = 0, attribSize = 0;
        GLenum attribType;

        glGetActiveAttrib(texturedMeshProgram.handle, i, ARRAY_COUNT(attribName)
            , &attribNameLength, &attribSize, &attribType, attribName);

        int attributeLocation = glGetAttribLocation(texturedMeshProgram.handle, attribName);

        texturedMeshProgram.vertexInputLayout.attributes.push_back({ (u8)attributeLocation,(u8)attribSize });
    }

    //Uniforms initialization

    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &app->maxUniformBufferSize);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &app->uniformBlockAlignment);

    app->buffer = CreateConstantBuffer(app->maxUniformBufferSize);

}

mat4 TransformScale(const vec3& scaleFactors)
{
    glm::mat4 transform = scale(scaleFactors);
    return transform;
}

mat4 TransformPositionScale(const vec3& pos, const vec3& scaleFactor)
{
    glm::mat4 transform = translate(pos);
    transform = scale(transform, scaleFactor);
    return transform;
}

void HandleInput(App* app)
{
    vec3 position = app->camera.GetPosition();

    if (app->input.keys[K_W] == BUTTON_PRESSED)
    {
        position.z += app->cameraMoveSpeed * app->deltaTime;
    }
    if (app->input.keys[K_S] == BUTTON_PRESSED)
    {
        position.z -= app->cameraMoveSpeed * app->deltaTime;
    }

    if (app->input.keys[K_D] == BUTTON_PRESSED)
    {
        position.x -= app->cameraMoveSpeed * app->deltaTime;
    }

    if (app->input.keys[K_A] == BUTTON_PRESSED)
    {
        position.x += app->cameraMoveSpeed * app->deltaTime;
    }

    app->camera.SetPosition(position);


}