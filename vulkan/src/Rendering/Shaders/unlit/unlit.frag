#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec2 inTexCoord;

layout ( set = 1, binding = 0 ) uniform MaterialUniform {
    vec4 color;
};

layout ( set = 1, binding = 1 ) uniform sampler2D uSampler;

layout ( location = 0 ) out vec4 outColor;

void main()
{
	outColor = color * texture( uSampler, inTexCoord );
    if ( outColor.a <= 0.01 ) {
        discard;
    }
}
