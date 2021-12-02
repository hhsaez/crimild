layout( location = 0 ) in vec3 inPosition;
layout( location = 1 ) in vec4 inColor;
layout( location = 2 ) in float inSize;
layout( location = 3 ) in float inAlive;

layout( set = 0, binding = 0 ) uniform RenderPassUniforms
{
    mat4 view;
    mat4 proj;
    vec2 viewport;
};

layout( set = 2, binding = 0 ) uniform GeometryUniforms
{
    mat4 model;
};

layout( location = 0 ) out vec4 outColor;

void main()
{
    gl_Position = proj * view * model * vec4( inPosition, 1.0 );

    float size = inSize;

    // Scale particle size depending on camera projection
    vec4 eyePos = view * model * vec4( inPosition, 1.0 );
    vec4 projectedCorner =
        proj
        * vec4(
            0.5 * size,
            0.5 * size,
            eyePos.z,
            eyePos.w );

    gl_PointSize = inAlive * viewport.x * projectedCorner.x / projectedCorner.w;

    outColor = inColor;
}
