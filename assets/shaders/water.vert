uniform float u_Time;

void main()
{
    vec4 position = gl_Vertex;

    vec2 uv = gl_MultiTexCoord0.xy;

    float wave =
        sin(uv.x * 0.15 + u_Time * 0.7) * 0.8 +
        cos(uv.y * 0.12 + u_Time * 0.5) * 0.6;

    position.y += wave;

    gl_Position = gl_ModelViewProjectionMatrix * position;

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}