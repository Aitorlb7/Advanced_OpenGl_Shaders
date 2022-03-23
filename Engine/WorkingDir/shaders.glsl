///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef TEXTURED_GEOMETRY

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


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.
