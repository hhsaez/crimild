#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 inPosition;

layout ( set = 1, binding = 0 ) uniform samplerCube uSampler;

layout ( location = 0 ) out vec4 outColor;

void main()
{
    outColor = vec4( texture( uSampler, inPosition ).rgb, 1.0 );
}
