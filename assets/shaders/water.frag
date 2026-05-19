#version 120

// Water fragment shader
// - Samples two scrolling layers from a tileable water texture
// - Combines layers to create foam and variation
// - Mixes a base deep/shallow color with texture detail for a pleasing result

uniform sampler2D u_WaterTexture;
uniform float u_Time;
uniform int u_DebugMode; // 0 = normal, 1 = show layerA

void main()
{
    // Compute base UV from vertex-provided texcoords (these are grid coords)
    // Scale down so the 32px texture tiles across the world reasonably
    vec2 baseUV = gl_TexCoord[0].xy * 0.12;

    // Two independent flow vectors for parallax motion — increased speed for visibility
    vec2 flowA = vec2(u_Time * 0.12, u_Time * 0.06);
    vec2 flowB = vec2(-u_Time * 0.08, u_Time * 0.09);

    // Sample the texture layers
    vec4 layerA = texture2D(u_WaterTexture, baseUV + flowA);
    vec4 layerB = texture2D(u_WaterTexture, baseUV * 1.8 + flowB);

    // Debug mode: output raw sampled layerA so we can verify texture binding & motion
    if (u_DebugMode == 1) {
        // Slightly boost brightness for visibility
        vec3 dbg = layerA.rgb * 1.6;
        gl_FragColor = vec4(dbg, 1.0);
        return;
    }

    // Derive a foam/shallowness factor from layer brightness (no 1/255 mistake here)
    float brightnessA = (layerA.r + layerA.g + layerA.b) / 3.0;
    float brightnessB = (layerB.r + layerB.g + layerB.b) / 3.0;
    float foam = clamp(brightnessA * 1.2 + brightnessB * 0.5, 0.0, 1.0);

    // Base color gradient between deep and shallow areas
    vec3 deepWater = vec3(0.02, 0.18, 0.28);
    vec3 shallowWater = vec3(0.15, 0.65, 0.85);
    vec3 waterColor = mix(deepWater, shallowWater, foam);

    // Use texture detail to modulate final color — no extreme scaling
    vec3 textureContribution = (layerA.rgb + layerB.rgb) * 0.5;
    vec3 finalRGB = mix(waterColor, textureContribution, 0.6);

    // Final alpha: mostly opaque so texture is visible; vertex tint still applies
    vec4 finalColor = vec4(finalRGB, 0.90);
    finalColor *= gl_Color;

    gl_FragColor = finalColor;
}