R"(

layout ( location = 0 ) CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
layout ( location = 4 ) CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;

CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;

void main()
{
	vTextureCoord = aTextureCoord;
	CRIMILD_GLSL_VERTEX_OUTPUT = vec4( aPosition, 1.0 );
}

)"

