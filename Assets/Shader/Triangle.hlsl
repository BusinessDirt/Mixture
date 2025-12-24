// -----------------------------------------------------------------------------
// STRUCTS
// -----------------------------------------------------------------------------
struct VS_Output
{
    float4 position : SV_POSITION; // Vertex position in screen space
    float4 color    : COLOR;       // Vertex color
};

// -----------------------------------------------------------------------------
// VERTEX SHADER
// -----------------------------------------------------------------------------
// We use 'uint id : SV_VertexID' to identify which vertex we are processing.
// No data is sent from the CPU; we hardcode the triangle here.
VS_Output VS_Main(uint id : SV_VertexID)
{
    VS_Output output;

    // 1. Define positions for 3 points of a triangle
    //    (X, Y, Z, W) -> W must be 1.0
    float4 positions[3] = {
        float4( 0.0f,  0.5f, 0.0f, 1.0f), // Top Center
        float4( 0.5f, -0.5f, 0.0f, 1.0f), // Bottom Right
        float4(-0.5f, -0.5f, 0.0f, 1.0f)  // Bottom Left
    };

    // 2. Define colors for those points
    //    (R, G, B, A)
    float4 colors[3] = {
        float4(1.0f, 0.0f, 0.0f, 1.0f), // Red
        float4(0.0f, 1.0f, 0.0f, 1.0f), // Green
        float4(0.0f, 0.0f, 1.0f, 1.0f)  // Blue
    };

    // 3. Assign the position and color based on the current Vertex ID (0, 1, or 2)
    output.position = positions[id];
    output.color    = colors[id];

    return output;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
float4 PS_Main(VS_Output input) : SV_TARGET
{
    // Return the interpolated color
    return input.color;
}
