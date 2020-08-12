#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec2 inTexCoord;

layout ( set = 0, binding = 0 ) uniform ConvolutionUniform {
    float kernel[ 9 ];
};

layout ( set = 0, binding = 1 ) uniform sampler2D uColorMap;

layout ( location = 0 ) out vec4 outColor;

const float offset = 1.0 / 300.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2( -offset, offset ), // top-left
        vec2( 0.0f, offset ), // top-center
        vec2( offset, offset ), // top-right
        vec2( -offset, 0.0 ), // center-left
        vec2( 0.0, 0.0 ), // center
        vec2( offset, 0.0 ), // center-right
        vec2( -offset, -offset ), // bottom-left
        vec2( 0.0, -offset ), // bottom-center
        vec2( offset, -offset ) // bottom-right
    );

    vec3 sampleTex[ 9 ];
    for (int i = 0; i < 9; ++i) {
        sampleTex[ i ] = texture( uColorMap, inTexCoord.st + offsets[ i ]).rgb;
    }

    vec3 color = vec3( 0 );
    for (int i = 0; i < 9; ++i) {
        color += sampleTex[ i ] * kernel[ i ];
    }

	outColor = vec4( color, 1.0 );
}
