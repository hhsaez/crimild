#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) out vec4 outColor;

void main()
{
	float d = gl_FragCoord.z;
	float near = 0.1;
	float far = 100.0;
	d = ( 2.0 * near * far ) / ( far + near - d * ( far - near ) );
	outColor = vec4( vec3( d / far ), 1.0 );
}

