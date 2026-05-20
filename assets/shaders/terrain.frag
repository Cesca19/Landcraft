#version 120

uniform sampler2D u_Splatmap;
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
    // 1. Water clipping
    float terrainZ = u_MinElevation + (gl_Color.a * u_ElevationRange);
    if (terrainZ <= u_WaterHeight) {
        discard;
    }

    // 2. Pure wireframe mode
    if (u_IsWireframe > 0.5) {
        gl_FragColor = vec4(gl_Color.rgb, 1.0);
        return; 
    }

    // --- 3. THE SUN (SOFT SHADOW CALCULATION) ---
    float penteX = dFdx(terrainZ);
    float penteY = dFdy(terrainZ);
    
    vec2 soleil = normalize(vec2(-1.0, 1.0));
    
    // 1. Scale the slope for subtler gradients
    vec2 intensitePente = vec2(penteX, penteY) * 10.0; 
    
    float lumiere = 1.0 + dot(intensitePente, soleil);
    
    // 2. AMBIENT LIGHT (the secret is here)
    // We clamp the minimum value to 0.8 instead of 0.4.
    // The strongest shadow will only remove 20% of the original color!
    lumiere = clamp(lumiere, 0.95, 1.05); 
    // ----------------------------------------

    // 4. Texture blending
    vec2 tileUV = gl_TexCoord[0].xy;
    vec2 splatUV = tileUV / u_MapSize;
    splatUV.y = 1.0 - splatUV.y;
    vec4 weights = texture2D(u_Splatmap, splatUV);
    
    vec4 colGrass = texture2D(u_TexGrass, tileUV);
    vec4 colSand  = texture2D(u_TexSand, tileUV);
    vec4 colRock  = texture2D(u_TexRock, tileUV);
    vec4 colSnow  = texture2D(u_TexSnow, tileUV);

    vec4 finalColor = vec4(1.0, 1.0, 1.0, 1.0);
    finalColor = mix(finalColor, colGrass, weights.r);
    finalColor = mix(finalColor, colSand,  weights.g);
    finalColor = mix(finalColor, colRock,  weights.b);
    finalColor = mix(finalColor, colSnow,  weights.a);
    
    // --- MAGIC: APPLYING SHADOW TO THE TEXTURE ---
    finalColor.rgb *= lumiere;
    
    // 5. Grid overlay (must be applied AFTER lighting to remain visible)
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