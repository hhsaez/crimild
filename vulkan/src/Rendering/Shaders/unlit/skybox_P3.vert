#version 450
#extension GL_ARB_separate_shader_objects : enable

layout ( location = 0 ) in vec3 inPosition;

layout ( binding = 0 ) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout ( location = 0 ) out vec3 fragPosition;

void main()
{
	gl_Position = ubo.proj * mat4( mat3( ubo.view ) ) * ubo.model * vec4( inPosition, 1.0 );
	gl_Position = gl_Position.xyww;
	fragPosition = inPosition;

	// Cubemaps are always computed on NDC, so we need to flip the Y-coordinate
	// for Vulkan in order to render it correctly
	fragPosition.y *= -1.0; 
}

