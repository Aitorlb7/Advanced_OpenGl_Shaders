#pragma once

#include "platform.h"

struct App;
struct Program;

namespace Skybox
{

	void SetUpSkyBoxBuffers();
	u32 CreateSkyboxTexture();
	void RenderSkybox(App* app, Program& SkyboxProgram);
	
};