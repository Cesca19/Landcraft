#version 120

uniform sampler2D u_SplatMap;
uniform sampler2D u_TexGrass;
uniform sampler2D u_TexSand;
uniform sampler2D u_TexRock;
uniform sampler2D u_TexSnow;

uniform float u_MinElevation;
uniform float u_ElevationRange;
uniform float u_WaterHeight;

uniform float u_IsWireframe;
uniform float u_ShowGrid;
uniform vec4 u_WireframeColor;
uniform vec2 u_MapSize;

// light control ---
// 0: 3d sun (Half-Lambert)
// 1: 2d sun (Fake Headlamp)
// uniform int u_LightingModel; 

void main() {
    int u_LightingModel = 1;
    // Wireframe mode
    if (u_IsWireframe > 0.5) {
        gl_FragColor = vec4(gl_Color.rgb, 1.0);
        return;
    }

    // Water clipping and height computing
    float terrainZ = u_MinElevation + (gl_Color.a * u_ElevationRange);
    if (terrainZ <= u_WaterHeight) {
        discard;
    }

    vec2 tileUV = gl_TexCoord[0].xy;

    float lightFactor;
    if (u_LightingModel == 0) {
        // 3d sun (Half-Lambert)
        vec3 dPdx_3d = vec3(dFdx(tileUV.x), dFdx(tileUV.y), dFdx(terrainZ));
        vec3 dPdy_3d = vec3(dFdy(tileUV.x), dFdy(tileUV.y), dFdy(terrainZ));
        vec3 normal = normalize(cross(dPdx_3d, dPdy_3d));
        if (normal.z < 0.0) {
            normal = -normal;
        }
        // 3d sun (X=1, Y=1)
        vec3 sunDirection = normalize(vec3(-1.0, -1.0, 3.5)); 
        // Half-Lambert : smooth lighting with no full shadow, to avoid harsh dark areas on steep slopes
        float lambert = dot(normal, sunDirection);
        float halfLambert = (lambert * 0.5) + 0.5; 
        lightFactor = 0.7 + (halfLambert * 0.4);
    } else {
        // 2d sun (Fake Headlamp)
        // This model ensures perfect readability from any camera angle.
        float slopeX = dFdx(terrainZ);
        float slopeY = dFdy(terrainZ);
        // The relief factor is based on the difference between the slopes in X and Y directions.
        float relief = (slopeY - slopeX) * 0.7;
        lightFactor = clamp(1.0 + relief, 0.8, 1.3);
    }

    // Texture blending
    vec2 splatUV = tileUV / u_MapSize;
    splatUV.y = 1.0 - splatUV.y;
    vec4 weights = texture2D(u_SplatMap, splatUV);
    vec4 colGrass = texture2D(u_TexGrass, tileUV);
    vec4 colSand  = texture2D(u_TexSand, tileUV);
    vec4 colRock  = texture2D(u_TexRock, tileUV);
    vec4 colSnow  = texture2D(u_TexSnow, tileUV);
    // Weight normalization and blending
    float wSum = weights.r + weights.g + weights.b + weights.a;
    vec4 texturesColor = colGrass * weights.r + colSand * weights.g + colRock * weights.b + colSnow * weights.a;
    // Fill the remaining area (if wSum < 1.0) with a white background
    float emptySpace = max(0.0, 1.0 - wSum);
    vec4 finalColor = texturesColor + vec4(1.0, 1.0, 1.0, 1.0) * emptySpace;

    // Apply lighting
    finalColor.rgb *= lightFactor;

    // Grid overlay
    if (u_ShowGrid > 0.5) {
        vec2 edgeDist = abs(fract(tileUV - 0.5) - 0.5);
        vec2 fw = fwidth(tileUV);
        float thickness = 1.5;
        float lineX = smoothstep(0.0, fw.x * thickness, edgeDist.x);
        float lineY = smoothstep(0.0, fw.y * thickness, edgeDist.y);
        float gridFactor = 1.0 - min(lineX, lineY);

        if (gridFactor > 0.0) {
            finalColor = mix(finalColor, u_WireframeColor, gridFactor * u_WireframeColor.a);
        }
    }

    gl_FragColor = finalColor * gl_Color;
    gl_FragColor.a = 1.0;
}