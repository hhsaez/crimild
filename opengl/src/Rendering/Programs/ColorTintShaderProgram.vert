R"(

CRIMILD_GLSL_ATTRIBUTE( 0 ) vec3 aPosition;
CRIMILD_GLSL_ATTRIBUTE( 4 ) vec2 aTextureCoord;

CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;

void main()
{
	vTextureCoord = aTextureCoord;
	CRIMILD_GLSL_VERTEX_OUTPUT = vec4( aPosition, 1.0 );
}

)"

