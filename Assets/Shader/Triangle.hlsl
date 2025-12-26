// SimpleTriangle.hlsl

struct VS_Output
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

// Add this attribute:
[shader("vertex")]
VS_Output VS_Main(uint id : SV_VertexID)
{
    VS_Output output;

    float4 positions[3] = {
        float4( 0.0f,  0.5f, 0.0f, 1.0f),
        float4( 0.5f, -0.5f, 0.0f, 1.0f),
        float4(-0.5f, -0.5f, 0.0f, 1.0f)
    };

    float4 colors[3] = {
        float4(1.0f, 0.0f, 0.0f, 1.0f),
        float4(0.0f, 1.0f, 0.0f, 1.0f),
        float4(0.0f, 0.0f, 1.0f, 1.0f)
    };

    output.position = positions[id];
    output.color    = colors[id];

    return output;
}

// Add this attribute:
[shader("pixel")]
float4 PS_Main(VS_Output input) : SV_TARGET
{
    return input.color;
}
