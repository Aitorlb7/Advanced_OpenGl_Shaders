#define _CRT_SECURE_NO_WARNINGS

#include "Skybox.h"
#include "engine.h"


#include "stb_image.h"
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION


	u32 SkyboxTexID = 0;
	u32 SkyboxVAO = 0;
	u32 SkyboxID = 0;

	std::vector<std::string> faces
	{
			"s_left.png",
			"s_right.png",
			"s_up.png",
			"s_down.png",
			"s_front.png",
			"s_back.png"

	}; //Add proper names

	float SkyboxVertices[108] = {

		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

void Skybox::SetUpSkyBoxBuffers()
{
	//glGenBuffers(1, (GLuint*)&(SkyboxVAO));
	//glBindBuffer(GL_ARRAY_BUFFER, SkyboxVAO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, SkyboxVertices, GL_STATIC_DRAW);


	u32 skyboxVBO;
	glGenVertexArrays(1, &SkyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(SkyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), &SkyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



}

u32 Skybox::CreateSkyboxTexture()
{
	glGenTextures(1, &SkyboxTexID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTexID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string textureFile = "Skybox/";
		textureFile.append(faces[i]);

		Image img = {};
		//if (stbi_is_hdr(textureFile.c_str()))
		//{
		//	stbi_set_flip_vertically_on_load(true);
		//	img.pixels = stbi_load(textureFile.c_str(), &img.size.x, &img.size.y, &img.nchannels, 0);
		//}

		stbi_set_flip_vertically_on_load(true);
		img.pixels = stbi_load(textureFile.c_str(), &img.size.x, &img.size.y, &img.nchannels, 0);


		if (img.pixels)
		{
			img.stride = img.size.x * img.nchannels;
			
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img.size.x, img.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.pixels);
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_FLOAT, img.size.x, img.size.y, 0, GL_FLOAT, GL_UNSIGNED_BYTE, img.pixels);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_FLOAT, img.size.x, img.size.y, 0, GL_RGB, GL_FLOAT, img.pixels);
			
			stbi_image_free(img.pixels);

		}
		else
		{
			ELOG("Could not open file %s", textureFile.c_str());
			stbi_image_free(img.pixels);
		}
		

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::RenderSkybox(App* app, u32 SkyboxProgramID)
{
	glDepthMask(GL_FALSE);

	glUseProgram(SkyboxProgramID);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	
	//mat4 modelMatrix = TransformPositionScale(app->camera.GetPosition(), vec3(50.f));
	//mat4 resultMatrix = glm::transpose(modelMatrix) * glm::transpose(app->camera.GetView()) * glm::transpose(app->camera.GetProjection());

	mat4 resultMatrix = glm::mat4(glm::mat3(app->camera.GetView())) * app->camera.GetProjection();

	GLint uinformLoc = glGetUniformLocation(SkyboxProgramID, "resultMatrix");
	glUniformMatrix4fv(uinformLoc, 1, GL_FALSE, glm::value_ptr(resultMatrix));

	glBindVertexArray(SkyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTexID);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, SkyboxVAO);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	//glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);
	glUseProgram(0);
}
