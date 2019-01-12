R"(

struct Light {
    vec3 ambient;
    vec3 diffuse;
	float hasDirection;
	vec3 direction;
	float hasPosition;
	vec3 position;
	float hasAttenuation;
	vec3 attenuation;
	float innerCutOff;
	float outerCutOff;
};

uniform int uLightCount;
uniform Light uLights[ MAX_LIGHTS ];

vec3 calcPhongLighting( vec3 P, vec3 N, vec3 E, vec3 MA, vec3 MD, vec3 MS, float MSh )
{
	vec3 accumAmbient = vec3( 0 );
	vec3 accumDiffuse = vec3( 0 );
	vec3 accumSpecular = vec3( 0 );

	int lightCount = min( uLightCount, MAX_LIGHTS );
	for ( int i = 0; i < lightCount; i++ ) {
		vec3 lAmbient = uLights[ i ].ambient;
		vec3 lDiffuse = uLights[ i ].diffuse;
		float hasDirection = uLights[ i ].hasDirection;
		vec3 lDirection = uLights[ i ].direction;
		float hasPosition = uLights[ i ].hasPosition;
		vec3 lPosition = uLights[ i ].position;
		float hasAttenuation = uLights[ i ].hasAttenuation;
		vec3 lAttenuation = uLights[ i ].attenuation;
		float lInnerCutOff = uLights[ i ].innerCutOff;
		float lOuterCutOff = uLights[ i ].outerCutOff;

		vec3 lVector = ( 1.0 - hasPosition ) * lDirection + hasPosition * ( lPosition - P );
		float lDistance = length( lVector );

		vec3 L = normalize( lVector );
		vec3 H = normalize( L + E );

		vec3 CA = uLights[ i ].ambient;
		vec3 CD = lDiffuse * max( 0.0, dot( N, L ) );
		vec3 CS = lDiffuse * pow( max( dot( N, H ), 0.0 ), MSh );

		// spotlight
		float isSpotlight = hasPosition * hasDirection;
		float theta = dot( L, normalize( -lDirection ) );
		float epsilon = lInnerCutOff - lOuterCutOff;
		float intensity = clamp( ( theta - lOuterCutOff ) / epsilon, 0.0, 1.0 );
		intensity = isSpotlight * intensity + 1.0 - isSpotlight;
		CD *= intensity;
		CS *= intensity;

		// attenuation
		float a = 1.0 / ( lAttenuation.x + lDistance * lAttenuation.y + ( lDistance * lDistance ) * lAttenuation.z );
		a = ( 1.0 - hasAttenuation ) + hasAttenuation * a;
		CA *= a;
		CD *= a;
		CS *= a;
		
		accumAmbient += CA;
		accumDiffuse += CD;
		accumSpecular += CS;
	}

	return accumAmbient * MA + accumDiffuse * MD + accumSpecular * MS;
}

)"

