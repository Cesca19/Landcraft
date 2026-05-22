uniform sampler2D u_WaterTexture;
uniform sampler2D u_NormalMap;
uniform float u_Time;

float wave(vec2 uv, vec2 dir, float speed, float scale, float phase)
{
    return sin(dot(uv, dir) * scale + u_Time * speed + phase);
}

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    vec2 bigUV = uv * 0.06;

    float w1 = wave(bigUV, vec2(1.0, 0.25), 0.9, 5.0, 0.0);
    float w2 = wave(bigUV, vec2(-0.7, 1.0), 0.6, 3.5, 1.2);
    float w3 = wave(bigUV, vec2(0.3, 1.0), 1.1, 6.0, 2.1);
    float w4 = wave(bigUV * 1.5, vec2(1.2, -0.5), 1.6, 12.0, 0.7);
    float w5 = wave(bigUV * 2.0, vec2(-0.3, 0.8), 2.8, 20.0, 4.3);
    float w6 = wave(bigUV * 3.0, vec2(0.6, 0.2), 3.5, 30.0, 3.9);

    float combined = (w1 * 0.9 + w2 * 0.6 + w3 * 0.8 + w4 * 0.35 + w5 * 0.2 + w6 * 0.15) / (0.9 + 0.6 + 0.8 + 0.35 + 0.2 + 0.15);

    // Increase overall amplitude but clamp extremes to avoid dark patches
    combined *= 1.3;
    combined = clamp(combined, -0.35, 1.15);

    // Add small high-frequency ripples (lower amplitude)
    float ripples = sin((uv.x + uv.y) * 140.0 + u_Time * 6.0) * 0.06;
    combined += ripples * 0.5;

    // Sample normal map to add organic variation (reduced influence)
    vec3 n = texture2D(u_NormalMap, uv * 2.0 + vec2(u_Time * 0.01, u_Time * 0.008)).rgb;
    vec2 nxy = (n.xy * 2.0 - 1.0) * 0.28;
    combined += (nxy.x + nxy.y) * 0.05;

    // Reduce distortion magnitude to avoid sampling outside texture or extreme darkening
    vec2 distortion;
    distortion.x = combined * 0.035;
    distortion.y = combined * 0.028;

    // Create a multi-directional local flow from wave layers to avoid single-direction drift
    vec2 flow = normalize(vec2(1.0, 0.25)) * w1 * 0.018
              + normalize(vec2(-0.7, 1.0)) * w2 * 0.012
              + normalize(vec2(0.3, 1.0)) * w3 * 0.02
              + normalize(vec2(1.2, -0.5)) * w4 * 0.009
              + normalize(vec2(-0.3, 0.8)) * w5 * 0.006
              + normalize(vec2(0.6, 0.2)) * w6 * 0.004;

    vec2 animatedUV = uv * 0.12 + distortion + flow;

    vec4 color = texture2D(u_WaterTexture, animatedUV);

    // Keep a bright base water color so the surface never falls into dark bands
    vec3 baseWaterColor = vec3(0.20, 0.55, 1.00);
    color.rgb = mix(baseWaterColor, color.rgb, 0.45);

    // Add low-frequency color patches so the water varies by area
    float patchNoise = sin(uv.x * 7.0 + u_Time * 0.15)
                     + sin(uv.y * 5.0 - u_Time * 0.12)
                     + sin((uv.x + uv.y) * 3.5 + u_Time * 0.08);
    patchNoise /= 3.0;
    patchNoise = patchNoise * 0.5 + 0.5;

    vec3 shallowTint = vec3(0.26, 0.72, 1.00);
    vec3 deeperTint   = vec3(0.15, 0.44, 0.92);
    vec3 patchColor   = mix(deeperTint, shallowTint, patchNoise);
    color.rgb = mix(color.rgb, patchColor, 0.22);

    // Use waves to slightly brighten crests instead of darkening troughs
    float crest = max(combined, 0.0);
    color.rgb += crest * vec3(0.06, 0.03, 0.01);

    // Blue boost only for positive wave peaks (avoid dark blue patches)
    float blueBoost = smoothstep(0.05, 0.9, combined) * 0.03;
    color.b += blueBoost;

    color.rgb = clamp(color.rgb, 0.0, 1.0);

    gl_FragColor = color;
}