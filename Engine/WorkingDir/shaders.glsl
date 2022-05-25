///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef QUAD

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 texCoords;


out vec2 _texCoords;

void main()
{
	_texCoords = texCoords;
	gl_Position = vec4(aPosition, 1.0);
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

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef TEXTURED_MESH

layout(binding = 0,std140) uniform GlobalParams 
{
	vec3 uCameraPosition;
	float nearPlane;
	float farPlane;
};

layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out vec3 vViewDir;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition,1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal,0.0));
	vViewDir = normalize(uCameraPosition - vPosition);
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition,1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vViewDir;


uniform sampler2D textureSampler;

layout (location = 0) out vec4 positionOut;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 diffuseOut;
layout (location = 3) out vec4 depthOut;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{
	float depth = LinearizeDepth( gl_FragCoord.z) / farPlane;

	positionOut = vec4(vPosition,1.0);
    diffuseOut = vec4(texture(textureSampler, vTexCoord).xyz,1.0);
    normalOut = vec4( normalize(vNormal),1.0);
    depthOut = vec4(vec3(depth),1.0);
}

#endif
#endif


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef DIRECTIONAL_LIGHT

struct Light
{
	vec3 color;
	vec3 direction;
	vec3 position;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	float nearPlane;
	float farPlane;

};

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	Light directional;
};


#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;
	vNormal = vec3(uWorldMatrix * vec4(aNormal,0.0));
	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vNormal;

uniform sampler2D positionOut;
uniform sampler2D normalOut;
uniform sampler2D diffuseOut;
uniform sampler2D depthOut;

layout(location = 4) out vec4 resultColor;

void main()
{
	
	vec3 diffuse = texture(diffuseOut, vTexCoord).xyz;
	vec3 normal = texture(normalOut, vTexCoord).xyz;
	vec3 position = texture(positionOut, vTexCoord).xyz;

	vec3 resultAmbient = directional.color * 0.2;
	vec3 resultDiffuse = directional.color * mix(vec3(0), diffuse, dot(normal, directional.direction)) * 0.7;

	vec3 viewDir = normalize(uCameraPosition - position);
	vec3 reflectDir = normalize(reflect(-normalize(directional.direction),  vNormal));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 40.0);
	vec3 resultSpecular =  spec * directional.color; 


	resultColor = vec4(resultAmbient + resultAmbient + resultSpecular, 1.0);
}

#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef POINT_LIGHT

struct Light
{
	vec3 color;
	vec3 direction;
	vec3 position;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	float nearPlane;
	float farPlane;
};

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	Light pointLight;
};

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;
out vec3 vViewDir;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal,0.0));
	vViewDir = normalize(uCameraPosition - vPosition);
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

uniform sampler2D positionOut;
uniform sampler2D normalOut;
uniform sampler2D diffuseOut;
uniform sampler2D depthOut;

layout(location = 4) out vec4 resultColor;

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vViewDir;


void main()
{

	vec3 diffuse = texture(diffuseOut, vTexCoord).xyz;
	vec3 normal = texture(normalOut, vTexCoord).xyz;
	vec3 position = texture(positionOut, vTexCoord).xyz;
	float depth = texture(depthOut, vTexCoord).x;

	vec3 lightDir = normalize(pointLight.position - vPosition);
	vec3 reflectDir = reflect(-lightDir,  normalize(vNormal));

	float diff = max(dot(normalize(vNormal), lightDir), 0.0);
	float spec = pow(max(dot(vViewDir, reflectDir), 0.0), 40.0);

	float lightDistance = length(pointLight.position - vPosition);
	
	float attenuation = 1.0 / (1.0 + 0.09 * lightDistance + 0.032 * (lightDistance * lightDistance)); 

	vec3 resultDiffuse = diff * pointLight.color * diffuse * 0.7;
	vec3 resultSpecular =  spec * pointLight.color; 
	vec3 resultAmbient = pointLight.color * 0.2;

	resultAmbient  *= attenuation;
	resultDiffuse  *= attenuation;
	resultSpecular *= attenuation;
				 
	resultColor += vec4((resultSpecular + resultDiffuse + resultAmbient),1.0);
}

#endif
#endif