// Triangle.hlsl

struct VS_Input
{
    // These semantics and locations must match the Vertex Input State
    // setup in the RHI backend.
    // For now, we assume implicit mapping:
    // Location 0: Position
    // Location 1: Color
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 color    : COLOR;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

[shader("vertex")]
VS_Output VS_Main(VS_Input input)
{
    VS_Output output;

    output.position = float4(input.position, 1.0f);
    output.color    = float4(input.color, 1.0f);

    return output;
}

[shader("pixel")]
float4 PS_Main(VS_Output input) : SV_TARGET
{
    return input.color;
}