uniform sampler2D u_WaterTexture;
uniform float u_Time;

float wave(vec2 uv, vec2 dir, float speed, float scale)
{
    return sin(dot(uv, dir) * scale + u_Time * speed);
}

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    vec2 bigUV = uv * 0.08;

    float w1 = wave(bigUV, vec2(1.0, 0.2), 0.7, 4.0);
    float w2 = wave(bigUV, vec2(-0.7, 1.0), 0.5, 3.0);
    float w3 = wave(bigUV, vec2(0.3, 1.0), 0.9, 5.0);

    float combined = (w1 + w2 + w3) / 3.0;

    vec2 distortion;
    distortion.x = combined * 0.03;
    distortion.y = combined * 0.02;

    vec2 animatedUV =
        uv * 0.12 +
        distortion +
        vec2(u_Time * 0.015, u_Time * 0.01);

    vec4 color =
        texture2D(u_WaterTexture, animatedUV);

    float light =
        0.85 + combined * 0.15;

    color.rgb *= light;

    color.b += 0.08;

    gl_FragColor = color;
}