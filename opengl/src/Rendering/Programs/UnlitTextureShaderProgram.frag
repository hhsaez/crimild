R"(

CRIMILD_GLSL_PRECISION_FLOAT_HIGH

struct Material {
   vec4 diffuse;
};

CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

uniform bool uUseColorMap;
uniform sampler2D uColorMap;
uniform Material uMaterial;

CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

void main( void ) 
{
	vec4 color = uUseColorMap ? CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord ) : vec4( 1.0 );
	color *= uMaterial.diffuse;

	CRIMILD_GLSL_FRAGMENT_OUTPUT = color;
}

)"

