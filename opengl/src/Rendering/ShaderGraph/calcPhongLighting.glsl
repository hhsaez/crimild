R"(

struct AmbientLight {
	vec3 ambient;
};

uniform int uAmbientLightCount;
uniform AmbientLight uAmbientLights[ MAX_LIGHTS ];

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
	vec3 direction;
	bool hasShadowMap;
	mat4 lightSpaceMatrix;
	vec4 shadowMapViewport;
};

uniform int uDirectionalLightCount;
uniform DirectionalLight uDirectionalLights[ MAX_LIGHTS ];

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
	vec3 position;
	vec3 attenuation;
};

uniform int uPointLightCount;
uniform PointLight uPointLights[ MAX_LIGHTS ];

struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
	vec3 direction;
	vec3 position;
	vec3 attenuation;
	float innerCutOff;
	float outerCutOff;
};

uniform int uSpotLightCount;
uniform SpotLight uSpotLights[ MAX_LIGHTS ];

uniform sampler2D uShadowAtlas;

vec3 calcPhongAmbientLighting( vec3 MA )
{
	vec3 accumAmbient = vec3( 0 );
	
	int lightCount = min( uAmbientLightCount, MAX_LIGHTS );
	for ( int i = 0; i < lightCount; i++ ) {
		accumAmbient += uAmbientLights[ i ].ambient;
	}

	return accumAmbient * MA;
}

vec3 calcPhongDirectionalLighting( vec3 P, vec3 N, vec3 E, vec3 MA, vec3 MD, vec3 MS, float MSh )
{
	vec3 accumAmbient = vec3( 0 );
	vec3 accumDiffuse = vec3( 0 );
	vec3 accumSpecular = vec3( 0 );

	int lightCount = min( uDirectionalLightCount, MAX_LIGHTS );
	for ( int i = 0; i < lightCount; i++ ) {
		vec3 lAmbient = uDirectionalLights[ i ].ambient;
		vec3 lDiffuse = uDirectionalLights[ i ].diffuse;
		vec3 lDirection = uDirectionalLights[ i ].direction;

		vec3 lVector = lDirection;
		float lDistance = length( lVector );

		vec3 L = normalize( lVector );
		vec3 H = normalize( L + E );

		vec3 CA = lAmbient;
		vec3 CD = lDiffuse * max( 0.0, dot( N, L ) );
		vec3 CS = lDiffuse * pow( max( dot( N, H ), 0.0 ), MSh );

		// shadow
		if ( uDirectionalLights[ i ].hasShadowMap ) {
		    vec4 lsPos = uDirectionalLights[ i ].lightSpaceMatrix * vec4( P, 1.0 );
			vec3 projPos = lsPos.xyz / lsPos.w;
			projPos = projPos * 0.5 + vec3( 0.5 );
			vec4 viewport = uDirectionalLights[ i ].shadowMapViewport;
			vec2 shadowUV = vec2( viewport.x + viewport.z * projPos.x, viewport.y + viewport.w * projPos.y );
#ifdef CRIMILD_PACK_FlOAT_TO_RGBA
			float d = texture( uShadowAtlas, shadowUV ).x;
#else
			vec4 depthRGBA = texture( uShadowAtlas, shadowUV );
			float d = dot( depthRGBA, vec4( 1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 16581375.0 ) );
#endif
			float z = projPos.z;
			z *= 0.99999;
			float shadow = z < d ? 1.0 : 0.0;
			CD *= shadow;
			CS *= shadow;
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
		vec3 lAmbient = uPointLights[ i ].ambient;
		vec3 lDiffuse = uPointLights[ i ].diffuse;
		vec3 lPosition = uPointLights[ i ].position;
		vec3 lAttenuation = uPointLights[ i ].attenuation;

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
		vec3 lAmbient = uSpotLights[ i ].ambient;
		vec3 lDiffuse = uSpotLights[ i ].diffuse;
		vec3 lDirection = uSpotLights[ i ].direction;
		vec3 lPosition = uSpotLights[ i ].position;
		vec3 lAttenuation = uSpotLights[ i ].attenuation;
		float lInnerCutOff = uSpotLights[ i ].innerCutOff;
		float lOuterCutOff = uSpotLights[ i ].outerCutOff;

		vec3 lVector = lPosition - P;
		float lDistance = length( lVector );

		vec3 L = normalize( lVector );
		vec3 H = normalize( L + E );

		vec3 CA = lAmbient;
		vec3 CD = lDiffuse * max( 0.0, dot( N, L ) );
		vec3 CS = lDiffuse * pow( max( dot( N, H ), 0.0 ), MSh );

		// spotlight
		float theta = dot( L, normalize( -lDirection ) );
		float epsilon = lInnerCutOff - lOuterCutOff;
		float intensity = clamp( ( theta - lOuterCutOff ) / epsilon, 0.0, 1.0 );
		CD *= intensity;
		CS *= intensity;

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

vec3 calcPhongLighting( vec3 P, vec3 N, vec3 E, vec3 MA, vec3 MD, vec3 MS, float MSh )
{
	return calcPhongAmbientLighting( MA )
		+ calcPhongDirectionalLighting( P, N, E, MA, MD, MS, MSh )
		+ calcPhongPointLighting( P, N, E, MA, MD, MS, MSh )
		+ calcPhongSpotLighting( P, N, E, MA, MD, MS, MSh );
}

)"

