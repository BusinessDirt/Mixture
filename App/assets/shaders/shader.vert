#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 outUV;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(push_constant) uniform Push 
{
	mat4 ModelMatrix;
    mat4 NormalMatrix;
} push;

void main() {
    vec4 positionWorld = push.ModelMatrix * vec4(inPosition, 1.0);
    gl_Position = ubo.ProjectionMatrix * ubo.ViewMatrix * positionWorld;
    
    fragColor = inColor;
    fragPosWorld = positionWorld.xyz;
    fragNormalWorld = normalize(mat3(push.NormalMatrix) * inNormal);
    outUV = inUV;
}
