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
uniform sampler2D depthSampler;
uniform sampler2D skyboxSampler;

layout(location=0) out vec4 color;

void main()
{
	color = texture(textureSampler, _texCoords);	


//	color = texture(skyboxSampler, _texCoords);	
//
//	float depth = texture(depthSampler , _texCoords).z;
//
//	if(depth != 0.0)
//	{
//		color = texture(textureSampler, _texCoords);	
//	}
//	else{
//		color = texture(skyboxSampler, _texCoords);	
//
//	}

}

#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef TEXTURED_MESH

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;


layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition,1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal,0.0));
	gl_Position = uWorldViewProjectionMatrix * uWorldMatrix * vec4(aPosition,1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vNormal;


uniform sampler2D textureSampler;

layout (location = 0) out vec4 finalOut;
layout (location = 1) out vec4 positionOut;
layout (location = 2) out vec4 normalOut;
layout (location = 3) out vec4 diffuseOut;
layout (location = 4) out vec4 depthOut;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	vec2 uCameraResolution;
	float nearPlane;
	float farPlane;

};


float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{
	float depth = LinearizeDepth( gl_FragCoord.z) / farPlane;

	positionOut = vec4(vPosition,1.0);
    diffuseOut = texture(textureSampler, vTexCoord);
    normalOut = vec4( normalize(vNormal),1.0);
    depthOut = vec4(vec3(depth),1.0);
	finalOut = diffuseOut;

}

#endif
#endif


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef DIRECTIONAL_LIGHT

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;


out vec2 vTexCoord;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

struct Light
{
	vec3 color;
	vec3 direction;
	vec3 position;
};

in vec2 vTexCoord;

uniform sampler2D positionOut;
uniform sampler2D normalOut;
uniform sampler2D diffuseOut;
uniform sampler2D depthOut;



layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	vec2 uCameraResolution;
	float nearPlane;
	float farPlane;

};

layout(binding = 2, std140) uniform LightParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	Light directional;
};

layout(location = 0) out vec4 resultColor;

void main()
{
	resultColor = vec4(1.0,1.0,1.0,1.0);

	vec3 diffuse = texture(diffuseOut, vTexCoord).rgb;
	vec3 normal = texture(normalOut, vTexCoord).rgb;
	vec3 position = texture(positionOut, vTexCoord).rgb;


	vec3 viewDir = normalize(uCameraPosition - position);
	vec3 reflectDir = reflect(-normalize(directional.direction),  normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	float diff = max(dot(normal, directional.direction),0.0);
	

	vec3 resultAmbient = directional.color * 0.1;
	vec3 resultDiffuse = directional.color * diff * 0.7;
	vec3 resultSpecular =  spec * directional.color * 0.3; 


	resultColor = vec4((resultAmbient + resultDiffuse + resultSpecular) * diffuse, 1.0);
}

#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef POINT_LIGHT

#if defined(VERTEX) ///////////////////////////////////////////////////


struct Light
{
	vec3 color;
	vec3 direction;
	vec3 position;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;


layout(binding = 2, std140) uniform LightParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	Light pointLight;
};

void main()
{
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////


struct Light
{
	vec3 color;
	vec3 direction;
	vec3 position;
};


uniform sampler2D positionOut;
uniform sampler2D normalOut;
uniform sampler2D diffuseOut;
uniform sampler2D depthOut;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	vec2 uCameraResolution;
	float nearPlane;
	float farPlane;
};

layout(binding = 2, std140) uniform LightParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	Light pointLight;
};


layout(location = 0) out vec4 resultColor;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{

	resultColor = vec4(0.0,0.0,0.0,0.0);

	vec2 vTexCoord = vec2(gl_FragCoord.x / uCameraResolution.x, gl_FragCoord.y / uCameraResolution.y);
	
	vec3 diffuse = texture(diffuseOut, vTexCoord).rgb;
	vec3 normal = texture(normalOut, vTexCoord).rgb;
	vec3 position = texture(positionOut, vTexCoord).rgb;
	float depth = texture(depthOut, vTexCoord).x;

	float vDepth = LinearizeDepth(gl_FragCoord.z) / farPlane;

	vec3 vViewDir = normalize(uCameraPosition - position);
	vec3 lightDir = normalize(pointLight.position - position);
	vec3 reflectDir = reflect(-lightDir,  normalize(normal));


	float lightDistance = length(pointLight.position - position);
	
	if(lightDistance < 7.0)
	{
		
		float diff = max(dot(normalize(normal), lightDir), 0.0);
		float spec = pow(max(dot(vViewDir, reflectDir), 0.0), 40.0);

		float attenuation = 1.0 / (1.0 + 0.09 * lightDistance + 0.032 * (lightDistance * lightDistance)); 

		vec3 resultDiffuse = diff * pointLight.color * 0.7;
		vec3 resultSpecular =  spec * pointLight.color * 0.3; 
		vec3 resultAmbient = pointLight.color * 0.2;

		resultAmbient  *= attenuation;
		resultDiffuse  *= attenuation;
		resultSpecular *= attenuation;
				 
		resultColor += vec4((resultSpecular + resultDiffuse + resultAmbient) * diffuse,1.0);
	}
}

#endif
#endif


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef SKYBOX

#if defined(VERTEX) ///////////////////////////////////////////////////

layout (location = 0) in vec3 position;

out vec3 TexCoord;

uniform mat4 resultMatrix;

void main()
{
    vec4 fullMatrix = resultMatrix * vec4(position, 1.0);
    gl_Position = fullMatrix.xyww;
    TexCoord = position;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

layout (location = 0) out vec4 color;

in vec3 TexCoord;

uniform samplerCube skybox;

void main()
{ 
	color = texture(skybox, TexCoord);
}

#endif
#endif


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef REFLECTION

#if defined(VERTEX) ///////////////////////////////////////////////////

layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;


out vec3 vNormal;
out vec3 vPosition;

layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

void main()
{
	vNormal = mat3(inverse(transpose(uWorldMatrix))) * normal;
	vPosition = vec3(uWorldMatrix * vec4(position, 1.0));
	gl_Position = uWorldViewProjectionMatrix * uWorldMatrix * vec4(position, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////


layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	vec2 uCameraResolution;
	float nearPlane;
	float farPlane;
};


in vec3 vPosition;
in vec3 vNormal;

uniform samplerCube skybox;

layout(location = 0) out vec4 resultColor;

void main()
{ 
   vec3 I = normalize(vPosition - uCameraPosition);
   vec3 R = reflect(I, normalize(vNormal));
   
   resultColor = vec4(texture(skybox, R).rgb, 1.0);
}

#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef NORMAL_MAPPING

#if defined(VERTEX) ///////////////////////////////////////////////


layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBiTangent;


layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 localTangent;
out vec3 localBiTangent;
out vec3 localNormal;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition,1.0));
	localTangent = aTangent;
	localBiTangent = aBiTangent;
	localNormal = aNormal;
	gl_Position = uWorldViewProjectionMatrix * uWorldMatrix * vec4(aPosition,1.0);

}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 localTangent;
in vec3 localBiTangent;
in vec3 localNormal;

uniform sampler2D textureSampler;
uniform sampler2D normalMap;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	vec2 uCameraResolution;
	float nearPlane;
	float farPlane;

};

layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

layout (location = 0) out vec4 finalOut;
layout (location = 1) out vec4 positionOut;
layout (location = 2) out vec4 normalOut;
layout (location = 3) out vec4 diffuseOut;
layout (location = 4) out vec4 depthOut;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{
	float depth = LinearizeDepth( gl_FragCoord.z) / farPlane;

	positionOut = vec4(vPosition,1.0);
    diffuseOut = texture(textureSampler, vTexCoord);
    depthOut = vec4(vec3(depth),1.0);
	finalOut = diffuseOut;

	mat3 TBN = mat3(normalize(localTangent),normalize(localBiTangent),normalize(localNormal));
	vec3 tangentNormal = texture(normalMap, vTexCoord).xyz * 2.0 - vec3(1.0);
	vec3 spaceNormal = TBN * tangentNormal;
	vec3 worldSpaceNormal = normalize(uWorldMatrix * vec4(spaceNormal, 0.0)).xyz;
	normalOut = vec4(worldSpaceNormal,1.0);
}
#endif
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef BUMP_MAPPING

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBiTangent;

layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vTangent;
out vec3 vBiTangent;
out vec3 localNormal;

void main()
{
	vTexCoord = aTexCoord;
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vTangent = aTangent;
	vBiTangent = aBiTangent;
	localNormal = aNormal;
	gl_Position = uWorldViewProjectionMatrix * uWorldMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vPosition;
in vec3 vTangent;
in vec3 vBiTangent;
in vec3 localNormal;

uniform sampler2D textureSampler;
uniform sampler2D normalMap;
uniform sampler2D bumpMap;

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	vec2 uCameraResolution;
	float nearPlane;
	float farPlane;
};

layout(binding = 1,std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
};

layout (location = 0) out vec4 finalOut;
layout (location = 1) out vec4 positionOut;
layout (location = 2) out vec4 normalOut;
layout (location = 3) out vec4 diffuseOut;
layout (location = 4) out vec4 depthOut;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}
void main()
{
	float depth = LinearizeDepth( gl_FragCoord.z) / farPlane;

	mat3 matrix = mat3(normalize(vTangent),normalize(vBiTangent),normalize(localNormal));
	vec3 viewDir = normalize((matrix * uCameraPosition) - (matrix * vPosition));
	float heightScale = 0.08f;

	float numLayers = mix(64.0f, 8.0f, abs(dot(vec3(0.0f,0.0f,1.0f), viewDir)));
	float layerDepth = 1.0f / numLayers;
	
	vec2 deltaUVs = (viewDir.xy / viewDir.z * heightScale) / numLayers;	
	vec2 UV = vTexCoord;
	float depthMap = 1.0f - texture(bumpMap, UV).r;
	float currentLayerDepth = 0.0f;
	while(currentLayerDepth < depthMap)
	{
		UV -= deltaUVs;
		currentLayerDepth += layerDepth;
		depthMap = 1.0f - texture(bumpMap, UV).r;
	}
	
	vec2 prevTexCoords = UV + deltaUVs;
	float afterDepth = depthMap - currentLayerDepth;
	float beforeDepth = 1.0f - texture(bumpMap, prevTexCoords).r - currentLayerDepth + layerDepth;
	float weight = afterDepth / (afterDepth - beforeDepth);
	UV = prevTexCoords * weight + UV * (1.0f - weight);
	
	if(UV.x > 1.0 || UV.y > 1.0 || UV.x < 0.0 || UV.y < 0.0) discard;
	
	diffuseOut = texture(textureSampler,UV);
	positionOut = vec4(vPosition,1.0);
	depthOut = vec4(vec3(depth),1.0);
	finalOut = diffuseOut;

	vec3 tangentSpaceNormal = texture(normalMap, UV).xyz *2.0 - vec3(1.0);
	vec3 localSpaceNormal = matrix * tangentSpaceNormal;
	vec3 worldSpaceNormal = normalize(uWorldMatrix * vec4(localSpaceNormal, 0.0)).xyz;
	normalOut = vec4(worldSpaceNormal,1.0);
}
#endif
#endif