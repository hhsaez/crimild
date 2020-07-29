#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 inColor;
layout ( location = 1 ) in vec2 inTexCoord;

layout ( set = 1, binding = 1 ) uniform sampler2D uDiffuseMap;
layout ( set = 1, binding = 2 ) uniform sampler2D uSpecularMap;

layout ( location = 0 ) out vec4 outColor;

void main()
{
    vec3 color = inColor * texture( uDiffuseMap, inTexCoord ).rgb;

	outColor = vec4( inColor, 1.0 );
}
