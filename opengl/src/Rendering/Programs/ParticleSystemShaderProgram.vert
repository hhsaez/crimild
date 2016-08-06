R"(

CRIMILD_GLSL_ATTRIBUTE vec3 aPosition;
CRIMILD_GLSL_ATTRIBUTE vec4 aColor;
CRIMILD_GLSL_ATTRIBUTE vec2 aTextureCoord;

uniform mat4 uPMatrix; 
uniform mat4 uVMatrix; 
uniform mat4 uMMatrix;

CRIMILD_GLSL_VARYING_OUT vec4 vColor;

void main()
{
	vColor = aColor;

	CRIMILD_GLSL_VERTEX_OUTPUT = uPMatrix * uVMatrix * uMMatrix * vec4( aPosition, 1.0 );
	CRIMILD_GLSL_POINT_SIZE = aTextureCoord.x;
}

)"

