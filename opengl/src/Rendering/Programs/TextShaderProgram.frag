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
    vec4 color = uMaterial.diffuse;
    if ( uUseColorMap ) {
        float c = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord ).r;
        color *= vec4( c, c, c, c );
    }
    
    if ( color.a < 0.1 ) {
        discard;
    }

	CRIMILD_GLSL_FRAGMENT_OUTPUT = color;
}

)"

