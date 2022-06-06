#define _CRT_SECURE_NO_WARNINGS

#include "Skybox.h"
#include "engine.h"


#include "stb_image.h"
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION


	u32 SkyboxTexID = 0;
	u32 SkyboxVAO = 0;

	float skyboxVertices[] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f,//        7--------6
		 1.0f, -1.0f,  1.0f,//       /|       /|
		 1.0f, -1.0f, -1.0f,//      4--------5 |
		-1.0f, -1.0f, -1.0f,//      | |      | |
		-1.0f,  1.0f,  1.0f,//      | 3------|-2
		 1.0f,  1.0f,  1.0f,//      |/       |/
		 1.0f,  1.0f, -1.0f,//      0--------1
		-1.0f,  1.0f, -1.0f
	};

	unsigned int skyboxIndices[] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};


	std::vector<std::string> faces
	{
			"s_left.png",
			"s_right.png",
			"s_up.png",
			"s_down.png",
			"s_front.png",
			"s_back.png"

	}; //Add proper names
void Skybox::SetUpSkyBoxBuffers()
{
	unsigned int skyboxVBO, skyboxEBO, TexCoordsBuffer;
	glGenVertexArrays(1, &SkyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(SkyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

u32 Skybox::CreateSkyboxTexture()
{
	glGenTextures(1, &SkyboxTexID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTexID);


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string textureFile = "Skybox/";
		textureFile.append(faces[i]);

		Image img = {};

		stbi_set_flip_vertically_on_load(false);
		unsigned char * data = stbi_load(textureFile.c_str(), &img.size.x, &img.size.y, &img.nchannels, 0);

		if (data)
		{			
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img.size.x, img.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		else
		{
			ELOG("Could not open file %s", textureFile.c_str());
			stbi_image_free(data);
		}
		

	}


	return SkyboxTexID;
}

void Skybox::RenderSkybox(App* app, Program& SkyboxProgram)
{
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glDisable(GL_BLEND);

	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);
	//glDepthFunc(GL_LEQUAL);
	
	glUseProgram(SkyboxProgram.handle);
	mat4 modelMatrix = TransformPositionScale(app->camera.GetPosition(), vec3(1.f));
	//mat4 resultMatrix = glm::transpose(modelMatrix) * glm::transpose(app->camera.GetView()) * glm::transpose(app->camera.GetProjection());
	//mat4 resultMatrix = modelMatrix * app->camera.GetView() * app->camera.GetProjection();

	//mat4 resultMatrix =  glm::mat4(glm::mat3(app->camera.GetView())) * app->camera.GetProjection();
	mat4 resultMatrix = modelMatrix * app->camera.GetView() * app->camera.GetProjection();

	GLuint uniformLoc = glGetUniformLocation(SkyboxProgram.handle, "resultMatrix");
	glUniformMatrix4fv(uniformLoc, 1, GL_TRUE, glm::value_ptr(resultMatrix));

	glBindVertexArray(SkyboxVAO);

	uniformLoc = glGetUniformLocation(SkyboxProgram.handle, "skybox");
	glUniform1i(uniformLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTexID);


	uniformLoc = glGetUniformLocation(SkyboxProgram.handle, "depthTexture");
	glUniform1i(uniformLoc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, app->GDepth);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	glDepthMask(GL_TRUE);
	//glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);


	glBindVertexArray(0);
	glUseProgram(0);
}
