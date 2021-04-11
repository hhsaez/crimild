#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec2 fragTexCoord;

layout ( binding = 1 ) uniform sampler2D texSampler;

layout ( location = 0 ) out vec4 outColor;

void main()
{
	outColor = texture( texSampler, fragTexCoord );
}

