#version 450

layout ( location = 0 ) in vec3 inPosition;

layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
    mat4 view;
	mat4 proj;
};

layout ( set = 2, binding = 0 ) uniform GeometryUniforms {
	mat4 model;
};

layout ( location = 0 ) out vec3 outPosition;

void main()
{
	gl_Position = proj * mat4( mat3( view ) ) * model * vec4( inPosition, 1.0 );
    gl_Position = gl_Position.xyww;
    outPosition = normalize( inPosition );
}
