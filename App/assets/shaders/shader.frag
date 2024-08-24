#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 UV;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = texture(texSampler, UV);
}
