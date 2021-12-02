#version 450

layout ( location = 0 ) in vec3 inPosition;
layout ( location = 1 ) in vec2 inTexCoord;

layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
    mat4 view;
	mat4 proj;
};

layout ( set = 2, binding = 0 ) uniform GeometryUniforms {
	mat4 model;
};

layout ( location = 0 ) out vec2 outTexCoord;

void main()
{
	gl_Position = proj * view * model * vec4( inPosition, 1.0 );
	outTexCoord = inTexCoord;
}
