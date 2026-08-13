// Default.slang

struct CameraData
{
    float4x4 viewProjection;
    float3 position;
    float padding;
};

struct MaterialData
{
    float4 albedoColor;
    float metallic;
    float roughness;
    float2 tiling;
    float3 emissionColor;
    float emissionIntensity;
};

struct PushConstants
{
    float4x4 model;
};

static const uint MaxSceneLights = 16;

struct SceneLightData
{
    float4 positionRange;
    float4 directionType;
    float4 colorIntensity;
    float4 spotAngles;
};

struct SceneLightingData
{
    uint4 header;
    SceneLightData lights[MaxSceneLights];
};

[[vk::binding(0, 0)]] ConstantBuffer<CameraData> u_Camera;
[[vk::binding(0, 1)]] ConstantBuffer<MaterialData> u_Material;
[[vk::binding(0, 2)]] ConstantBuffer<SceneLightingData> u_SceneLighting;

[[vk::push_constant]]
PushConstants pushConstants;

struct VS_Input
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    [[vk::location(0)]] float3 worldPosition : TEXCOORD0;
    [[vk::location(1)]] float3 worldNormal : TEXCOORD1;
};

[shader("vertex")]
VS_Output VS_Main(VS_Input input)
{
    VS_Output output;

    float4 worldPos = mul(pushConstants.model, float4(input.position, 1.0f));
    output.position = mul(u_Camera.viewProjection, worldPos);
    output.worldPosition = worldPos.xyz;
    output.worldNormal = normalize(mul((float3x3)pushConstants.model, input.normal));

    return output;
}

[shader("pixel")]
float4 PS_Main(VS_Output input) : SV_TARGET
{
    float3 normal = normalize(input.worldNormal);
    const float3 viewDirection = normalize(u_Camera.position - input.worldPosition);
    const float roughness = clamp(u_Material.roughness, 0.04f, 1.0f);
    const float metallic = saturate(u_Material.metallic);

    const float3 albedo = u_Material.albedoColor.rgb;
    const float3 dielectricF0 = float3(0.04f, 0.04f, 0.04f);
    const float3 F0 = lerp(dielectricF0, albedo, metallic);
    const float3 ambient = albedo * 0.08f;
    const float3 emission = u_Material.emissionColor * u_Material.emissionIntensity;
    float3 directLighting = float3(0.0f, 0.0f, 0.0f);

    const uint lightCount = min(u_SceneLighting.header.x, MaxSceneLights);
    for (uint i = 0; i < lightCount; ++i)
    {
        const SceneLightData light = u_SceneLighting.lights[i];
        const uint lightType = uint(light.directionType.w);
        float3 lightDirection;
        float attenuation = 1.0f;

        if (lightType == 0)
        {
            lightDirection = normalize(-light.directionType.xyz);
        }
        else
        {
            const float3 toLight = light.positionRange.xyz - input.worldPosition;
            const float distanceToLight = length(toLight);
            lightDirection = toLight / max(distanceToLight, 0.0001f);
            const float rangeFactor = saturate(1.0f - distanceToLight / max(light.positionRange.w, 0.0001f));
            // Intensity/range are editor-friendly units, not physical candela;
            // smooth range falloff keeps the default point light clearly visible.
            attenuation = rangeFactor * rangeFactor;

            if (lightType == 2)
            {
                const float cone = dot(normalize(-light.directionType.xyz), lightDirection);
                attenuation *= smoothstep(light.spotAngles.x, 1.0f, cone);
            }
        }

        const float NdotL = saturate(dot(normal, lightDirection));
        const float3 halfDirection = normalize(viewDirection + lightDirection);
        const float NdotH = saturate(dot(normal, halfDirection));
        const float3 specular = F0 * pow(NdotH, lerp(128.0f, 4.0f, roughness));
        const float3 diffuse = albedo * (1.0f - metallic) * NdotL;
        directLighting += (diffuse + specular * NdotL) * light.colorIntensity.rgb
            * light.colorIntensity.w * attenuation;
    }

    return float4(ambient + directLighting + emission, u_Material.albedoColor.a);
}
