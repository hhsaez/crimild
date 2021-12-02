layout( location = 0 ) in vec3 inPosition;
layout( location = 1 ) in vec3 inColor;

layout( set = 0, binding = 0 ) uniform RenderPassUniforms
{
    mat4 view;
    mat4 proj;
};

layout( set = 2, binding = 0 ) uniform GeometryUniforms
{
    mat4 model;
};

layout( location = 0 ) out vec3 outColor;

void main()
{
    gl_Position = proj * view * model * vec4( inPosition, 1.0 );
    gl_PointSize = 20.0;
    outColor = inColor;
}
