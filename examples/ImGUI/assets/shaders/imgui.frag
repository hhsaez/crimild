#version 450 core

layout ( location = 0 ) in vec4 vColor;
layout ( location = 1 ) in vec2 vTexCoord;

layout ( binding = 0 ) uniform sampler2D uTexture;

layout ( location = 0 ) out vec4 FragColor;

void main()
{
	FragColor = vColor * texture( uTexture, vTexCoord );
}
