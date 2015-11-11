R"(

CRIMILD_GLSL_PRECISION_FLOAT_HIGH

CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;

uniform sampler2D uColorMap;

uniform float uTintValue = 1.0;
uniform vec4 uTint = vec4( 0.4392156863, 0.2588235294, 0.07843137255, 1.0 );

CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

void main( void ) 
{
	vec4 color = CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord );
    
    if ( uTintValue > 0.0 ) {
        float gray = ( color.r + color.g + color.b ) / 3.0;
        vec3 grayscale = vec3( gray, gray, gray );
        color = vec4( ( gray < 0.5 ? 2.0 * uTint.r * grayscale.x : 1.0 - 2.0 * ( 1.0 - grayscale.x ) * ( 1.0 - uTint.r ) ),
                     ( gray < 0.5 ? 2.0 * uTint.g * grayscale.y : 1.0 - 2.0 * ( 1.0 - grayscale.y ) * ( 1.0 - uTint.g ) ),
                     ( gray < 0.5 ? 2.0 * uTint.b * grayscale.z : 1.0 - 2.0 * ( 1.0 - grayscale.z ) * ( 1.0 - uTint.b ) ),
                     1.0 );
        color.rgb = grayscale + uTintValue * ( color.rgb - grayscale );
    }
    
	CRIMILD_GLSL_FRAGMENT_OUTPUT = color;
}

)"

