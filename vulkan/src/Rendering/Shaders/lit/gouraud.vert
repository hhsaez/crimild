#version 450

layout ( location = 0 ) in vec3 inPosition;
layout ( location = 1 ) in vec3 inNormal;
layout ( location = 2 ) in vec2 inTexCoord;

layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
    mat4 view;
	mat4 proj;
};

struct LightProps {
    uint type;
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 color;
    vec4 attenuation;
    vec4 cutoff;
};

layout ( set = 0, binding = 1 ) uniform Lighting {
    LightProps ambientLights[ 1 ];
    uint ambientLightCount;
    LightProps directionalLights[ 2 ];
    uint directionalLightCount;
    LightProps pointLights[ 10 ];
    uint pointLightCount;
    LightProps spotLights[ 4 ];
    uint spotLightCount;
} lighting;

layout ( set = 1, binding = 0 ) uniform Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
} material;

layout ( set = 2, binding = 0 ) uniform GeometryUniforms {
	mat4 model;
};

layout ( location = 0 ) out vec3 outColor;
layout ( location = 1 ) out vec2 outTexCoord;

vec3 ambient( vec3 lAmbient )
{
    return lAmbient * material.ambient.rgb;
}

vec3 diffuse( vec3 N, vec3 L )
{
    float d = max( dot( N, L ), 0.0 );
    return vec3( d ) * material.diffuse.rgb;
}

vec3 specular( vec3 N, vec3 L, vec3 E, vec3 P )
{
    vec3 V = normalize( E - P );
    vec3 R = reflect( -L, N );
    float s = pow( max( dot( V, R ), 0.0 ), material.shininess );
    return vec3( s ) * material.specular.rgb;
}

float attenuation( float d, vec3 K )
{
    return 1.0 / ( K.x + d * K.y + d * d * K.z );
}

void main()
{
    vec3 N = normalize( mat3( transpose( inverse( model ) ) ) * inNormal );
    vec3 P = vec3( model * vec4( inPosition, 1.0 ) );
    vec3 E = inverse( view )[ 3 ].xyz;

    vec3 lightContribution = vec3( 0 );

    // ambient lights
    for ( uint i = 0; i < lighting.ambientLightCount; ++i ) {
        vec3 A = ambient( lighting.ambientLights[ i ].ambient.rgb );
        lightContribution += A;
    }

    // point lights
    for ( uint i = 0; i < lighting.pointLightCount; ++i ) {
        vec3 L = lighting.pointLights[ i ].position.xyz - inPosition;
        float dist = length( L );
        L = normalize( L );
        vec3 Lc = lighting.pointLights[ i ].color.rgb;
        vec3 A = ambient( lighting.pointLights[ i ].ambient.rgb );
        vec3 D = diffuse( N, L );
        vec3 S = specular( N, L, E, P );
        float lAtt = attenuation( dist, lighting.pointLights[ i ].attenuation.xyz );
        lightContribution += Lc * lAtt * ( A + D + S );
    }

    // directional lights
    for ( uint i = 0; i < lighting.directionalLightCount; ++i ) {
        vec3 L = normalize( -lighting.directionalLights[ i ].direction.xyz );
        vec3 Lc = lighting.directionalLights[ i ].color.rgb;
        vec3 A = ambient( lighting.directionalLights[ i ].ambient.rgb );
        vec3 D = diffuse( N, L );
        vec3 S = specular( N, L, E, P );
        lightContribution += Lc * ( A + D + S );
    }

    // spot light
    for ( uint i = 0; i < lighting.spotLightCount; ++i ) {
        vec3 L = lighting.spotLights[ i ].position.xyz - inPosition;
        float dist = length( L );
        L = normalize( L );
        float theta = dot( L, normalize( -lighting.spotLights[ i ].direction.xyz ) );
        float epsilon = lighting.spotLights[ i ].cutoff.x - lighting.spotLights[ i ].cutoff.y;
        float intensity = clamp( ( theta - lighting.spotLights[ i ].cutoff.y ) / epsilon, 0.0, 1.0 );
        vec3 Lc = lighting.spotLights[ i ].color.rgb;
        vec3 A = ambient( lighting.spotLights[ i ].ambient.rgb );
        vec3 D = intensity * diffuse( N, L );
        vec3 S = intensity * specular( N, L, E, P );
        float lAtt = attenuation( dist, lighting.spotLights[ i ].attenuation.xyz );
        lightContribution += Lc * lAtt * ( A + D + S );
    }

	gl_Position = proj * view * model * vec4( inPosition, 1.0 );

	outColor = lightContribution;
    outTexCoord = inTexCoord;
}
