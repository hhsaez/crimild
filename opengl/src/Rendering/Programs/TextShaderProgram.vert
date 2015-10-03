R"(

CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;

uniform mat4 uPMatrix; 
uniform mat4 uVMatrix; 
uniform mat4 uMMatrix;

CRIMILD_GLSL_VARYING_OUT vec2 vTextureCoord;

void main()
{
	vTextureCoord = aTextureCoord;
	CRIMILD_GLSL_VERTEX_OUTPUT = uPMatrix * uVMatrix * uMMatrix * vec4( aPosition, 1.0 );
}

)"

