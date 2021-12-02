#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 inPosition;

layout ( set = 0, binding = 0) uniform RenderPassUniform
{
    mat4 view;
	mat4 proj;
};

layout ( set = 1, binding = 0 ) uniform ModelUniform {
    mat4 model;
};

void main()
{
	// Extrude along normal
	gl_Position = proj * view * model * vec4( 1.1 * inPosition, 1.0 );
}
