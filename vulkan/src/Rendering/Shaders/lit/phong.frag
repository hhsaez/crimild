#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 inNormal;
layout ( location = 1 ) in vec3 inPosition;
layout ( location = 2 ) in vec3 inEyePosition;
layout ( location = 3 ) in vec3 inLightPosition;

layout ( set = 1, binding = 0 ) uniform Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
} material;

layout ( location = 0 ) out vec4 outColor;

vec3 ambient()
{
    return vec3( 1.0 ) * material.ambient.rgb;
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

void main()
{
    vec3 N = normalize( inNormal );
    vec3 P = inPosition;
    vec3 E = inEyePosition;
    vec3 L = normalize( inLightPosition - inPosition );

    vec3 lightColor = vec3( 1.0 );

	outColor = vec4( lightColor * ( ambient() + diffuse( N, L ) + specular( N, L, E, P ) ), 1.0 );
}
