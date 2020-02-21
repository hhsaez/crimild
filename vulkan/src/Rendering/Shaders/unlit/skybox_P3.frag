#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 fragPosition;

layout ( binding = 1 ) uniform samplerCube texSampler;

layout ( location = 0 ) out vec4 outColor;

void main()
{
	outColor = texture( texSampler, fragPosition );
}

