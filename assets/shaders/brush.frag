uniform sampler2D currentTexture;
uniform vec4 validBounds;
uniform vec4 outlineColor;
uniform float outlineThickness;
uniform vec2 textureSize;
uniform int renderMode;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    if (uv.x < validBounds.x || uv.x > validBounds.z ||
        uv.y < validBounds.y || uv.y > validBounds.w)
    {
        discard;
    }

    vec4 texColor = texture2D(currentTexture, uv);
    float centerAlpha = texColor.a;
    vec2 texel = vec2(1.0 / textureSize.x, 1.0 / textureSize.y);
    vec2 o = texel * outlineThickness;

    // Neighbors (8 directions) : robust GLSL for older drivers.
    float aL  = texture2D(currentTexture, uv - vec2(o.x, 0.0)).a;
    float aR  = texture2D(currentTexture, uv + vec2(o.x, 0.0)).a;
    float aU  = texture2D(currentTexture, uv - vec2(0.0, o.y)).a;
    float aD  = texture2D(currentTexture, uv + vec2(0.0, o.y)).a;
    float aUL = texture2D(currentTexture, uv - vec2(o.x, o.y)).a;
    float aUR = texture2D(currentTexture, uv + vec2(o.x, -o.y)).a;
    float aDL = texture2D(currentTexture, uv + vec2(-o.x, o.y)).a;
    float aDR = texture2D(currentTexture, uv + vec2(o.x, o.y)).a;

    float maxA = max(max(max(aL, aR), max(aU, aD)), max(max(aUL, aUR), max(aDL, aDR)));
    maxA = max(maxA, centerAlpha);

    if (maxA <= 0.005) discard;

    float outlineWeight = smoothstep(0.02, 0.35, maxA - centerAlpha);

    if (renderMode == 1) {
        // Outline only
        vec4 outCol = outlineColor;
        outCol.a = outlineColor.a * outlineWeight;
        if (outCol.a <= 0.001) discard;
        gl_FragColor = outCol;
        return;
    }

    // Fill only
    float fillAlpha = smoothstep(0.02, 0.2, centerAlpha) * gl_Color.a;
    vec4 fill = vec4(texColor.rgb * gl_Color.rgb, fillAlpha);
    if (fill.a <= 0.001) discard;
    vec4 finalColor = fill;

    gl_FragColor = finalColor;
}
