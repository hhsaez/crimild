#version 450 core

layout ( location = 0 ) in vec2 aPosition;
layout ( location = 1 ) in vec2 aTexCoord;
layout ( location = 2 ) in vec4 aColor;

layout ( binding = 0 ) uniform TransformBuffer {
    vec2 scale;
	vec2 translate;
} ubo;

layout ( location = 0 ) out vec4 vColor;
layout ( location = 1 ) out vec2 vTexCoord;

void main()
{
	gl_Position = vec4( aPosition * ubo.scale * ubo.translate, 0.0, 1.0 );
	vColor = aColor;
	vTexCoord = aTexCoord;
}

