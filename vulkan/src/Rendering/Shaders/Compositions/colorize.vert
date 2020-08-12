#version 450
#extension GL_ARB_separate_shader_objects : enable

vec2 positions[6] = vec2[](
    vec2( -1.0, 1.0 ),
    vec2( -1.0, -1.0 ),
    vec2( 1.0, -1.0 ),

    vec2( -1.0, 1.0 ),
    vec2( 1.0, -1.0 ),
    vec2( 1.0, 1.0 )
);

vec2 texCoords[6] = vec2[](
    vec2( 0.0, 0.0 ),
    vec2( 0.0, 1.0 ),
    vec2( 1.0, 1.0 ),

    vec2( 0.0, 0.0 ),
    vec2( 1.0, 1.0 ),
    vec2( 1.0, 0.0 )
);

layout ( location = 0 ) out vec2 outTexCoord;

void main()
{
	gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
    outTexCoord = texCoords[ gl_VertexIndex ];
}
