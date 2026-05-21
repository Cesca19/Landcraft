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

void main() {
    // wireframe mode
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

    // 3D sun and shadow calculation
    vec3 dPdx = vec3(dFdx(tileUV.x), dFdx(tileUV.y), dFdx(terrainZ));
    vec3 dPdy = vec3(dFdy(tileUV.x), dFdy(tileUV.y), dFdy(terrainZ));
    vec3 normal = normalize(cross(dPdx, dPdy));
    
    if (normal.z < 0.0) {
        normal = -normal;
    }
    
    vec3 sunDirection = normalize(vec3(-1.0, -1.0, 2.5));
    float lightFactor = dot(normal, sunDirection);
    lightFactor = clamp(0.8 + (lightFactor * 0.3), 0.95, 1.05);

    // Texture blending
    vec2 splatUV = tileUV / u_MapSize;
    splatUV.y = 1.0 - splatUV.y;
    vec4 weights = texture2D(u_SplatMap, splatUV);

    vec4 colGrass = texture2D(u_TexGrass, tileUV);
    vec4 colSand  = texture2D(u_TexSand, tileUV);
    vec4 colRock  = texture2D(u_TexRock, tileUV);
    vec4 colSnow  = texture2D(u_TexSnow, tileUV);

    // Clean, direct blending (guaranteed by the C++ cleanup)
    float wSum = weights.r + weights.g + weights.b + weights.a;

    // Sum the textures
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