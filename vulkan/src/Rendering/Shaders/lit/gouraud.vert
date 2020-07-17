#version 450

layout ( location = 0 ) in vec3 inPosition;
layout ( location = 1 ) in vec3 inNormal;
layout ( location = 2 ) in vec3 inTexCoord;

layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
    mat4 view;
	mat4 proj;
};

layout ( set = 0, binding = 1 ) uniform Light {
    vec4 position;
} light;

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
    vec3 N = normalize( mat3( transpose( inverse( model ) ) ) * inNormal );
    vec3 P = vec3( model * vec4( inPosition, 1.0 ) );
    vec3 E = inverse( view )[ 3 ].xyz;
    vec3 L = normalize( light.position.xyz - P );

    vec3 lightColor = vec3( 1.0 );

	gl_Position = proj * view * model * vec4( inPosition, 1.0 );
	outColor = lightColor * ( ambient() + diffuse( N, L ) + specular( N, L, E, P ) );
}
