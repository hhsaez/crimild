R"(

CRIMILD_GLSL_PRECISION_FLOAT_HIGH

struct Light {
    float lightType;
    vec3 position;
    vec3 attenuation;
    vec3 direction;
    vec4 color;
    float outerCutoff;
    float innerCutoff;
    float exponent;
    vec4 ambient;
};

const float LIGHT_TYPE_POINT = 0.0;
const float LIGHT_TYPE_DIRECTIONAL = 1.0;
const float LIGHT_TYPE_SPOT = 2.0;

struct Material {
   vec4 ambient;
   vec4 diffuse;
   vec4 specular;
   float shininess;
   float emissive;
};

CRIMILD_GLSL_VARYING_IN vec4 vWorldVertex;
CRIMILD_GLSL_VARYING_IN vec3 vWorldNormal;
CRIMILD_GLSL_VARYING_IN vec3 vWorldTangent;
CRIMILD_GLSL_VARYING_IN vec3 vWorldBiTangent;
CRIMILD_GLSL_VARYING_IN vec3 vViewVec;
CRIMILD_GLSL_VARYING_IN vec2 vTextureCoord;
CRIMILD_GLSL_VARYING_IN vec4 vLightSpacePosition;

uniform int uLightCount;
uniform Light uLights[ 4 ];
uniform Material uMaterial;

uniform sampler2D uColorMap;
uniform bool uUseColorMap;
uniform sampler2D uNormalMap;
uniform bool uUseNormalMap;
uniform sampler2D uSpecularMap;
uniform bool uUseSpecularMap;
uniform sampler2D uShadowMap;
uniform bool uUseShadowMap;

CRIMILD_GLSL_DECLARE_FRAGMENT_OUTPUT

void main( void )
{
    // vWorldNormal gets interpolated when passed to the fragment shader
    // we need to re-normalize it again
    vec3 normal = normalize( vWorldNormal );
    
    vec4 outColor = vec4(0.0);
    
    vec4 color = uUseColorMap ? CRIMILD_GLSL_FN_TEXTURE_2D( uColorMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    color *= uMaterial.diffuse;
    if ( color.a == 0.0 ) {
	   discard;
    }

    if ( uMaterial.emissive > 0.0 ) {
        CRIMILD_GLSL_FRAGMENT_OUTPUT = vec4( color.rgb, 1.0 );
        return;
    }

    if ( uLightCount == 0 ) {
        CRIMILD_GLSL_FRAGMENT_OUTPUT = color;
        return;
    }
    
    vec4 specularColor = uUseSpecularMap ? CRIMILD_GLSL_FN_TEXTURE_2D( uSpecularMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    specularColor *= uMaterial.specular;
    
    outColor.rgb = uMaterial.ambient.rgb;
    outColor.a = color.a;
    
    float shadowFactor = 1.0;
    if ( uUseShadowMap ) {
        vec3 projCoord = vLightSpacePosition.xyz / vLightSpacePosition.w;
        vec3 uvShadowMap = vec3( 0.5 ) + 0.5 * projCoord;
        float d = CRIMILD_GLSL_FN_TEXTURE_2D( uShadowMap, uvShadowMap.xy ).x;
        float z = uvShadowMap.z;
        //z *= 0.999; // fixes acne
        //z += 0.0009; // fixes peter-panning
        if ( d < z ) {
            shadowFactor = 0.5;
        }
    }

    for ( int i = 0; i < 4; i++ ) {
        if ( i >= uLightCount ) {
            break;
        }

        outColor.rgb += uLights[ i ].ambient.rgb;

        vec3 lightVec;
        if ( uLights[ i ].lightType == LIGHT_TYPE_DIRECTIONAL ) {
            lightVec = -uLights[ i ].direction;
        }
        else {
            lightVec = uLights[ i ].position - vWorldVertex.xyz;
        }
        lightVec = normalize( lightVec );

        vec3 halfVector = -normalize( reflect( lightVec, vWorldNormal ) );
        vec3 eyeVector = normalize( vViewVec );
        
        if ( uUseNormalMap ) {
            vec3 temp;
            vec3 lightDir = lightVec;
            
            temp.x = dot( lightVec, vWorldTangent );
            temp.y = dot( lightVec, vWorldBiTangent );
            temp.z = dot( lightVec, vWorldNormal );
            lightVec = normalize( temp );
            
            temp.x = dot( halfVector, vWorldTangent );
            temp.y = dot( halfVector, vWorldBiTangent );
            temp.z = dot( halfVector, vWorldNormal );
            halfVector = normalize( temp );
            
            temp.x = dot( eyeVector, vWorldTangent );
            temp.y = dot( eyeVector, vWorldBiTangent );
            temp.z = dot( eyeVector, vWorldNormal );
            eyeVector = normalize( temp );
                    
            normal = 2.0 * CRIMILD_GLSL_FN_TEXTURE_2D( uNormalMap, vTextureCoord ).xyz - 1.0;
            normal = normalize( normal );
        }
        
        float l = dot( normal, lightVec );
        if ( l > 0.0 ) {
            float spotlight = 1.0;
            if ( uLights[ i ].lightType == LIGHT_TYPE_SPOT && uLights[ i ].outerCutoff > 0.0 ) {
                spotlight = max( -dot( lightVec, uLights[ i ].direction ), 0.0 );
                float spotlightFade = clamp( ( uLights[ i ].outerCutoff - spotlight ) / ( uLights[ i ].outerCutoff - uLights[ i ].innerCutoff ), 0.0, 1.0 );
                spotlight = pow( spotlight * spotlightFade, uLights[ i ].exponent );
            }
            
            float s = pow( max( dot( halfVector, eyeVector ), 0.0 ), uMaterial.shininess );
            float d = distance( vWorldVertex.xyz, uLights[ i ].position );
            float a = 1.0 / ( uLights[ i ].attenuation.x + ( uLights[ i ].attenuation.y * d ) + ( uLights[ i ].attenuation.z * d * d ) );
            
            outColor.rgb += shadowFactor * ( ( color.rgb * l ) + ( specularColor.rgb * s ) ) * uLights[ i ].color.rgb * a * spotlight;
        }
    }
    
    CRIMILD_GLSL_FRAGMENT_OUTPUT = outColor;

}

)"

