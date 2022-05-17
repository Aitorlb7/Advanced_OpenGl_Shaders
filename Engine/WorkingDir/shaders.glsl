///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef QUAD

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 position;
layout(location=1) in vec2 texCoords;


out vec2 _texCoords;

void main()
{
	_texCoords = texCoords;
	gl_Position = vec4(position, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 _texCoords;

uniform sampler2D textureSampler;

layout(location=0) out vec4 color;

void main()
{
	color = texture(textureSampler, _texCoords);
}

#endif
#endif


#ifdef TEXTURED_MESH 

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	vec3 position;
};


#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(binding = 0,std140) uniform GlobalParams 
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[10];
};

layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out vec3 vViewDir;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition,1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal,0.0));
	vViewDir = uCameraPosition - vPosition;
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition,1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vViewDir;

uniform sampler2D textureSampler;

layout(binding = 0,std140) uniform GlobalParams 
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[10];
};

layout(location=0) out vec4 oColor;

void main()
{
	vec4 outputColor = vec4(0.0);

	for(unsigned int i = 0; i < uLightCount; i++)
    {
		if(uLight[i].type == 0)
		{
			vec3 lightDir = normalize(uLight[i].direction);

			vec3 reflectDir = reflect(lightDir, normalize(vNormal));
		
			//Specular
			float spec = pow(max(dot(vViewDir, reflectDir), 0.0), 32);
			vec4 resultSpecular =  0.1 * spec * vec4(uLight[i].color,1.0); 

			//Diffuse  
			float diff = max(dot(normalize(vNormal), lightDir), 0.0);
			vec4 resultDiffuse = diff * vec4(uLight[i].color,1.0);

			outputColor += resultSpecular + resultDiffuse;
		}
		else
		{
			 vec3 lightDir = normalize(uLight[i].position - vPosition);
			 vec3 reflectDir = reflect(-lightDir, vNormal);
			 
			//Diffuse  
			float diff = max(dot(normalize(vNormal), lightDir), 0.0);
			vec4 resultDiffuse = diff * vec4(uLight[i].color,1.0);

			//Specular
			float spec = pow(max(dot(vViewDir, reflectDir), 0.0), 32);
			vec4 resultSpecular =  0.1 * spec * vec4(uLight[i].color,1.0); 

			// attenuation
			 float distance = length(uLight[i].position - vPosition);

			float attenuation = 1.0 / (1.0 + 0.09 * distance + 
  			     0.032 * (distance * distance)); 
				 
			outputColor += (resultSpecular + resultDiffuse) * attenuation ;
		}


       
    }

	oColor = texture(textureSampler, vTexCoord);
		
	oColor = oColor * outputColor;

}

#endif
#endif