#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec2 inTexCoord;

layout ( set = 0, binding = 0 ) uniform Uniforms {
    vec4 color;
};

layout ( set = 0, binding = 1 ) uniform sampler2D uColorMap;

layout ( location = 0 ) out vec4 outColor;

void main()
{
    vec4 base = texture( uColorMap, inTexCoord );
	outColor = vec4( color.rgb * base.rgb, color.a );
}
