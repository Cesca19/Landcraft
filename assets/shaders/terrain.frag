uniform sampler2D u_Splatmap;
uniform sampler2D u_TexGrass;
uniform sampler2D u_TexSand;
uniform sampler2D u_TexRock;
uniform sampler2D u_TexSnow;

uniform float u_MinElevation;
uniform float u_ElevationRange;
uniform float u_WaterHeight;

uniform vec2 u_MapSize; 

uniform float u_IsWireframe;

void main() {
    // Decode altitude : Minimum + (Percentage * Range)
    float terrainZ = u_MinElevation + (gl_Color.a * u_ElevationRange);
    // Clip the terrain if it is underwater
    if (terrainZ <= u_WaterHeight) {
        discard;
    }

    if (u_IsWireframe > 0.5) {
        gl_FragColor = vec4(gl_Color.rgb, 1.0);
        return;
    }

    // get the tile UV coordinates from the vertex shader
    vec2 tileUV = gl_TexCoord[0].xy; 
    
    // normalize tileUV to [0,1] range based on the map size
    vec2 splatUV = tileUV / u_MapSize; 
    // Flip the y-coordinate for the splatmap  because  OpenGl and SFML have opposite y-axis directions for textures
    splatUV.y = 1.0 - splatUV.y; 
    
    // Read the splatmap pixel at splatUV coordinates to get the blending weights for each texture
    vec4 weights = texture2D(u_Splatmap, splatUV);

    // we ask the graphic card for the color of each texture at the tileUV coordinates
    vec4 colGrass = texture2D(u_TexGrass, tileUV);
    vec4 colSand  = texture2D(u_TexSand, tileUV);
    vec4 colRock  = texture2D(u_TexRock, tileUV);
    vec4 colSnow  = texture2D(u_TexSnow, tileUV);
    
    // we initialize the final color to white
    vec4 finalColor = vec4(1.0, 1.0, 1.0, 1.0);
    // the function mix(a, b, w) performs a linear interpolation 
    // between a and b using w as the interpolant.
    finalColor = mix(finalColor, colGrass, weights.r);
    finalColor = mix(finalColor, colSand,  weights.g);
    finalColor = mix(finalColor, colRock,  weights.b);
    finalColor = mix(finalColor, colSnow,  weights.a);
    
    // we multiply the final color by the vertex color (which is white in our case) to get the final output color
    gl_FragColor = finalColor * gl_Color;
    gl_FragColor.a = 1.0; // Set alpha to 1 for opaque terrain
}