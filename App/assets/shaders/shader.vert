#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 outUV;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
} ubo;

layout(push_constant) uniform Push 
{
	mat4 ModelMatrix;
} push;

void main() {
    gl_Position = ubo.ProjectionMatrix * ubo.ViewMatrix * push.ModelMatrix * vec4(inPosition, 1.0);
    fragColor = inColor;
    outUV = inUV;
}
