// Standard vertex shader
void main() {
    // Transform the vertex position to screen space
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    // Pass texture coordinates (the UVs we clapped to the grid)
    // gl_MultiTexCoord0 represent the texture_coords set in the tilemap
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    
    // Pass color to the fragment shader
    gl_FrontColor = gl_Color;
}