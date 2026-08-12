// Default.slang

struct PushConstants
{
    float4x4 model;
    float4x4 viewProjection;
};

[[vk::push_constant]]
PushConstants pushConstants;

struct VS_Input
{
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

    float4 worldPos = mul(pushConstants.model, float4(input.position, 1.0f));
    output.position = mul(pushConstants.viewProjection, worldPos);
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f); // White color

    return output;
}

[shader("pixel")]
float4 PS_Main(VS_Output input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f); // Render white
}
