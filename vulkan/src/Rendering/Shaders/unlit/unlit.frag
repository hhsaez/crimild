#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec2 inTexCoord;

layout ( set = 1, binding = 0 ) uniform RenderPassUniforms {
    vec4 color;
};

layout ( set = 1, binding = 1 ) uniform sampler2D uSampler;

layout ( location = 0 ) out vec4 outColor;

void main()
{
	outColor = vec4( color.rgb * texture( uSampler, inTexCoord ).rgb, 1.0 );
}
