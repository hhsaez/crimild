R"(

struct AmbientLight {
	vec4 ambient;
};

layout ( std140 ) uniform uAmbientLightsBlock {
	AmbientLight uAmbientLights[ MAX_LIGHTS ];
};

uniform int uAmbientLightCount;
uniform int uAmbientLightIndices[ MAX_LIGHTS ];

struct DirectionalLight {
    vec4 ambient;
    vec4 diffuse;
	vec4 direction;
	mat4 lightSpaceMatrix;
	vec4 shadowMapViewport;
	vec2 shadowMinMaxBias;
	bool castShadows;
	float padding;
};

layout ( std140 ) uniform uDirectionalLightsBlock {
	DirectionalLight uDirectionalLights[ MAX_LIGHTS ];
};

uniform int uDirectionalLightCount;
uniform int uDirectionalLightIndices[ MAX_LIGHTS ];

struct PointLight {
    vec4 ambient;
    vec4 diffuse;
	vec4 position;
	vec4 attenuation;
};

layout ( std140 ) uniform uPointLightsBlock {
	PointLight uPointLights[ MAX_LIGHTS ];
};

uniform int uPointLightCount;
uniform int uPointLightIndices[ MAX_LIGHTS ];

struct SpotLight {
    vec4 ambient;
    vec4 diffuse;
	vec4 direction;
	vec4 position;
	vec4 attenuation;
    mat4 lightSpaceMatrix;
    vec4 shadowMapViewport;
    vec2 shadowMinMaxBias;
	float innerCutOff;
	float outerCutOff;
    bool castShadows;
	float padding[ 3 ];
};

layout ( std140 ) uniform uSpotLightsBlock {
	SpotLight uSpotLights[ MAX_LIGHTS ];
};

uniform int uSpotLightCount;
uniform int uSpotLightIndices[ MAX_LIGHTS ];

uniform sampler2D uShadowAtlas;

vec3 calcPhongAmbientLighting( vec3 MA )
{
	vec3 accumAmbient = vec3( 0 );
	
	int lightCount = min( uAmbientLightCount, MAX_LIGHTS );
	for ( int i = 0; i < lightCount; i++ ) {
		int lightIdx = uAmbientLightIndices[ i ];
		accumAmbient += uAmbientLights[ lightIdx ].ambient.rgb;
	}

	return accumAmbient * MA;
}

float calcDirectionalShadow( mat4 lightSpaceMatrix, vec3 P, vec3 N, vec3 L, vec4 viewport, vec2 minMaxBias, sampler2D shadowAtlas )
{
    vec4 lsPos = lightSpaceMatrix * vec4( P, 1.0 );
    vec3 projPos = lsPos.xyz / lsPos.w;
    projPos = projPos * 0.5 + vec3( 0.5 );
    vec2 shadowUV = vec2( viewport.x + viewport.z * projPos.x, viewport.y + viewport.w * projPos.y );
    float z = projPos.z;
    if ( z >= 1.0 ) {
        return 0.0;
    }

    float bias = max( minMaxBias.x * ( 1.0 - dot( N, L ) ), minMaxBias.x );
    float shadow = 0.0;
	ivec2 shadowTextureSize = textureSize( shadowAtlas, 0 );
    vec2 texelSize = vec2( 1.0 / float( shadowTextureSize.x ), 1.0 / float( shadowTextureSize.y ) );
    for ( float x = -1.0; x <= 1.0; x += 1.0 ) {
        for ( float y = -1.0; y <= 1.0; y += 1.0 ) {
            vec2 uv = shadowUV + vec2( x, y ) * texelSize;
#ifdef CRIMILD_PACK_FlOAT_TO_RGBA
            float d = texture( shadowAtlas, uv ).r;
#else
            vec4 depthRGBA = texture( shadowAtlas, uv );
            float d = dot( depthRGBA, vec4( 1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 16581375.0 ) );
#endif
            shadow += z - bias > d ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

vec3 calcPhongDirectionalLighting( vec3 P, vec3 N, vec3 E, vec3 MA, vec3 MD, vec3 MS, float MSh )
{
	vec3 accumAmbient = vec3( 0 );
	vec3 accumDiffuse = vec3( 0 );
	vec3 accumSpecular = vec3( 0 );

	int lightCount = min( uDirectionalLightCount, MAX_LIGHTS );
	for ( int i = 0; i < lightCount; i++ ) {
		int lightIdx = uDirectionalLightIndices[ i ];
		vec3 lAmbient = uDirectionalLights[ lightIdx ].ambient.rgb;
		vec3 lDiffuse = uDirectionalLights[ lightIdx ].diffuse.rgb;
		vec3 lDirection = uDirectionalLights[ lightIdx ].direction.xyz;

		vec3 lVector = lDirection;
		float lDistance = length( lVector );

		vec3 L = normalize( lVector );
		vec3 H = normalize( L + E );

		vec3 CA = lAmbient;
		vec3 CD = lDiffuse * max( 0.0, dot( N, L ) );
		vec3 CS = lDiffuse * pow( max( dot( N, H ), 0.0 ), MSh );

		// shadow
		if ( uDirectionalLights[ lightIdx ].castShadows ) {
            float shadow = calcDirectionalShadow(
                uDirectionalLights[ lightIdx ].lightSpaceMatrix,
                P,
                N,
                L,
                uDirectionalLights[ lightIdx ].shadowMapViewport,
                uDirectionalLights[ lightIdx ].shadowMinMaxBias,
                uShadowAtlas
            );
            CD *= ( 1.0 - shadow );
            CS *= ( 1.0 - shadow );
		}

		accumAmbient += CA;
		accumDiffuse += CD;
		accumSpecular += CS;
	}

	return accumAmbient * MA + accumDiffuse * MD + accumSpecular * MS;
}

vec3 calcPhongPointLighting( vec3 P, vec3 N, vec3 E, vec3 MA, vec3 MD, vec3 MS, float MSh )
{
	vec3 accumAmbient = vec3( 0 );
	vec3 accumDiffuse = vec3( 0 );
	vec3 accumSpecular = vec3( 0 );

	int lightCount = min( uPointLightCount, MAX_LIGHTS );
	for ( int i = 0; i < lightCount; i++ ) {
		int lightIdx = uPointLightIndices[ i ];
		vec3 lAmbient = uPointLights[ lightIdx ].ambient.rgb;
		vec3 lDiffuse = uPointLights[ lightIdx ].diffuse.rgb;
		vec3 lPosition = uPointLights[ lightIdx ].position.rgb;
		vec3 lAttenuation = uPointLights[ lightIdx ].attenuation.xyz;

		vec3 lVector = lPosition - P;
		float lDistance = length( lVector );

		vec3 L = normalize( lVector );
		vec3 H = normalize( L + E );

		vec3 CA = lAmbient;
		vec3 CD = lDiffuse * max( 0.0, dot( N, L ) );
		vec3 CS = lDiffuse * pow( max( dot( N, H ), 0.0 ), MSh );

		// attenuation
		float a = 1.0 / ( lAttenuation.x + lDistance * lAttenuation.y + ( lDistance * lDistance ) * lAttenuation.z );
		CA *= a;
		CD *= a;
		CS *= a;
		
		accumAmbient += CA;
		accumDiffuse += CD;
		accumSpecular += CS;
	}

	return accumAmbient * MA + accumDiffuse * MD + accumSpecular * MS;
}

vec3 calcPhongSpotLighting( vec3 P, vec3 N, vec3 E, vec3 MA, vec3 MD, vec3 MS, float MSh )
{
	vec3 accumAmbient = vec3( 0 );
	vec3 accumDiffuse = vec3( 0 );
	vec3 accumSpecular = vec3( 0 );

	int lightCount = min( uSpotLightCount, MAX_LIGHTS );
	for ( int i = 0; i < lightCount; i++ ) {
		int lightIdx = uSpotLightIndices[ i ];
		vec3 lAmbient = uSpotLights[ lightIdx ].ambient.rgb;
		vec3 lDiffuse = uSpotLights[ lightIdx ].diffuse.rgb;
		vec3 lDirection = uSpotLights[ lightIdx ].direction.xyz;
		vec3 lPosition = uSpotLights[ lightIdx ].position.xyz;
		vec3 lAttenuation = uSpotLights[ lightIdx ].attenuation.xyz;
		float lInnerCutOff = uSpotLights[ lightIdx ].innerCutOff;
		float lOuterCutOff = uSpotLights[ lightIdx ].outerCutOff;

		vec3 lVector = lPosition - P;
		float lDistance = length( lVector );

		vec3 L = normalize( lVector );
		vec3 H = normalize( L + E );

        // attenuation
        float a = 1.0 / ( lAttenuation.x + lDistance * lAttenuation.y + ( lDistance * lDistance ) * lAttenuation.z );
        if ( a <= 0.1 ) {
            // early cut
            continue;
        }

        float dNL = dot( N, L );
        if ( dNL <= 0.0 ) {
            // early cut
            continue;
        }

        // shadow
        float shadow = 0.0;
        if ( uSpotLights[ lightIdx ].castShadows ) {
            shadow = calcDirectionalShadow(
                uSpotLights[ lightIdx ].lightSpaceMatrix,
                P,
                N,
                L,
                uSpotLights[ lightIdx ].shadowMapViewport,
                uSpotLights[ lightIdx ].shadowMinMaxBias,
                uShadowAtlas
            );
        }

        if ( shadow == 1.0 ) {
            // early cut
            continue;
        }

        vec3 CA = lAmbient;
        vec3 CD = lDiffuse * max( 0.0, dNL );
        vec3 CS = lDiffuse * pow( max( dot( N, H ), 0.0 ), MSh );

        CA *= a;
        CD *= a;
        CS *= a;

        CD *= ( 1.0 - shadow );
        CS *= ( 1.0 - shadow );

        // spotlight
        float theta = dot( L, normalize( -lDirection ) );
        float epsilon = lInnerCutOff - lOuterCutOff;
        float intensity = clamp( ( theta - lOuterCutOff ) / epsilon, 0.0, 1.0 );
        CD *= intensity;
        CS *= intensity;

		accumAmbient += CA;
		accumDiffuse += CD;
		accumSpecular += CS;
	}

	return accumAmbient * MA + accumDiffuse * MD + accumSpecular * MS;
}

vec3 calcPhongLighting( vec3 P, vec3 N, vec3 E, vec3 MA, vec3 MD, vec3 MS, float MSh )
{
	return calcPhongAmbientLighting( MA )
		+ calcPhongDirectionalLighting( P, N, E, MA, MD, MS, MSh )
		+ calcPhongPointLighting( P, N, E, MA, MD, MS, MSh )
		+ calcPhongSpotLighting( P, N, E, MA, MD, MS, MSh );
}

)"

