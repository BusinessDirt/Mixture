// Default.slang

struct CameraData
{
    float4x4 viewProjection;
};

struct MaterialData
{
    float4 albedoColor;
    float metallic;
    float roughness;
    float2 padding;
};

struct PushConstants
{
    float4x4 model;
};

[[vk::binding(0, 0)]] ConstantBuffer<CameraData> u_Camera;
[[vk::binding(0, 1)]] ConstantBuffer<MaterialData> u_Material;

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
    output.position = mul(u_Camera.viewProjection, worldPos);
    output.color = u_Material.albedoColor;

    return output;
}

[shader("pixel")]
float4 PS_Main(VS_Output input) : SV_TARGET
{
    return input.color;
}
