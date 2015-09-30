R"(                                           
    struct Light {
       vec3 position;
       vec3 attenuation;
       vec3 direction;
       vec4 color;
       float outerCutoff;
       float innerCutoff;
       float exponent;
    };

    struct Material {
       vec4 ambient;
       vec4 diffuse;
       vec4 specular;
       float shininess;
       float emissive;
    };

    in vec4 vWorldVertex;
    in vec3 vWorldNormal;
    in vec3 vWorldTangent;
    in vec3 vWorldBiTangent;
    in vec3 vViewVec;
    in vec2 vTextureCoord;
    in vec4 vPosition;

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
                                             
    uniform float uLinearDepthConstant;

    out vec4 vFragColor;
                                             
    float unpack( vec4 color )
    {
        const vec4 bitShifts = vec4(1.0,
                                    1.0 / 255.0,
                                    1.0 / (255.0 * 255.0),
                                    1.0 / (255.0 * 255.0 * 255.0));
        return dot( color, bitShifts );
    }


    void main( void )
    {
        // vWorldNormal gets interpolated when passed to the fragment shader
        // we need to re-normalize it again
        vec3 normal = normalize( vWorldNormal );
        
    	vec4 color = uUseColorMap ? texture( uColorMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    	color *= uMaterial.diffuse;
    	if ( color.a == 0.0 ) {
    		discard;
    	}

        if ( uMaterial.emissive > 0.0f ) {
            vFragColor.rgb = color.rgb;
            vFragColor.a = 1.0f;
            return;
        }

        if ( uLightCount == 0 ) {
            vFragColor = color;
            return;
        }
        
    	vec4 specularColor = uUseSpecularMap ? texture( uSpecularMap, vTextureCoord ) : vec4( 1.0, 1.0, 1.0, 1.0 );
    	specularColor *= uMaterial.specular;
        
    	vFragColor.rgb = uMaterial.ambient.rgb;
    	vFragColor.a = color.a;
        
        for ( int i = 0; i < 4; i++ ) {
            if ( i >= uLightCount ) {
                break;
            }
            
            vec3 lightVec = normalize( uLights[ i ].position - vWorldVertex.xyz );
            vec3 halfVector = -normalize( reflect( lightVec, vWorldNormal ) );
            
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
                
                normal = 2.0 * texture( uNormalMap, vTextureCoord ).xyz - 1.0;
                normal = normalize( normal );
            }
            
            float l = dot( normal, lightVec );
            if ( l > 0.0 ) {
                float spotlight = 1.0;
                if ( ( uLights[ i ].direction.x != 0.0 ) || ( uLights[ i ].direction.y != 0.0 ) || ( uLights[ i ].direction.z != 0.0 ) ) {
                    spotlight = max( -dot( lightVec, uLights[ i ].direction ), 0.0 );
                    float spotlightFade = clamp( ( uLights[ i ].outerCutoff - spotlight ) / ( uLights[ i ].outerCutoff - uLights[ i ].innerCutoff ), 0.0, 1.0 );
                    spotlight = pow( spotlight * spotlightFade, uLights[ i ].exponent );
                }
                
                float s = pow( max( dot( halfVector, uUseNormalMap ? normal : vViewVec ), 0.0 ), uMaterial.shininess );
                float d = distance( vWorldVertex.xyz, uLights[ i ].position );
                float a = 1.0 / ( uLights[ i ].attenuation.x + ( uLights[ i ].attenuation.y * d ) + ( uLights[ i ].attenuation.z * d * d ) );
                
                vFragColor.rgb += ( ( color.rgb * l ) + ( specularColor.rgb * s ) ) * uLights[ i ].color.rgb * a * spotlight;
            }
        }
        
        if ( uUseShadowMap ) {
            vec3 depth = vPosition.xyz / vPosition.w;
            depth.z = length( vWorldVertex.xyz - uLights[ 0 ].position ) * uLinearDepthConstant;
            float shadow = 1.0;
            //depth.z *= 0.99;
            vec4 shadowColor = texture( uShadowMap, depth.xy );
            float shadowDepth = unpack( shadowColor );
            if ( depth.z > shadowDepth ) {
                shadow = 0.5;
            }
            
            vFragColor = clamp( vec4( vFragColor.rgb * shadow, vFragColor.a ), 0.0, 1.0 );
            vFragColor.a = 1.0;
        }

    }

)"

