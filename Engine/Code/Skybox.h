#pragma once

#include "platform.h"

struct App;

namespace Skybox
{

	void SetUpSkyBoxBuffers();
	u32 CreateSkyboxTexture();
	void RenderSkybox(App* app, u32 SkyboxProgramID);
	
};