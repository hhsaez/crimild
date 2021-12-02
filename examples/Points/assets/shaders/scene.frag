layout( location = 0 ) in vec3 inColor;

layout( location = 0 ) out vec4 outColor;

void main()
{
    vec2 coord = gl_PointCoord - vec2( 0.5 );
    if ( length( coord ) > 0.5 ) {
        discard;
    }

    outColor = vec4( inColor, 1.0 );
}
