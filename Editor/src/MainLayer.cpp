#include "MainLayer.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/Scene/Components.hpp"
#include "Mixture/Scene/Entity.hpp"

#include <cstring>

namespace Mixture
{
    struct Vertex
    {
        glm::vec3 Position;
    };

    struct alignas(16) CameraData
    {
        glm::mat4 ViewProjection;
    };

    void MainLayer::OnAttach()
    {
        OPAL_INFO("Client", "MainLayer::OnAttach()");

        // Initialize active Scene
        m_Scene = Scene::Create("Main Scene");

        // Create default scene entities if empty
        auto camera = m_Scene->CreateEntity("Main Camera");
        camera.AddComponent<CameraComponent>();
        camera.GetComponent<TransformComponent>().Position = { 0.0f, 0.0f, 5.0f };

        auto light = m_Scene->CreateEntity("Point Light");
        light.AddComponent<LightComponent>();
        light.GetComponent<TransformComponent>().Position = { 2.0f, 4.0f, 2.0f };

        auto cube = m_Scene->CreateEntity("Cube");
        cube.AddComponent<MeshRendererComponent>();

        // Create 3D Cube primitive vertex buffer (36 vertices - 6 faces)
        Vertex cubeVertices[] = {
            // Front face (Z = +0.5)
            { { -0.5f, -0.5f,  0.5f } },
            { {  0.5f, -0.5f,  0.5f } },
            { {  0.5f,  0.5f,  0.5f } },
            { {  0.5f,  0.5f,  0.5f } },
            { { -0.5f,  0.5f,  0.5f } },
            { { -0.5f, -0.5f,  0.5f } },

            // Right face (X = +0.5)
            { {  0.5f, -0.5f,  0.5f } },
            { {  0.5f, -0.5f, -0.5f } },
            { {  0.5f,  0.5f, -0.5f } },
            { {  0.5f,  0.5f, -0.5f } },
            { {  0.5f,  0.5f,  0.5f } },
            { {  0.5f, -0.5f,  0.5f } },

            // Back face (Z = -0.5)
            { {  0.5f, -0.5f, -0.5f } },
            { { -0.5f, -0.5f, -0.5f } },
            { { -0.5f,  0.5f, -0.5f } },
            { { -0.5f,  0.5f, -0.5f } },
            { {  0.5f,  0.5f, -0.5f } },
            { {  0.5f, -0.5f, -0.5f } },

            // Left face (X = -0.5)
            { { -0.5f, -0.5f, -0.5f } },
            { { -0.5f, -0.5f,  0.5f } },
            { { -0.5f,  0.5f,  0.5f } },
            { { -0.5f,  0.5f,  0.5f } },
            { { -0.5f,  0.5f, -0.5f } },
            { { -0.5f, -0.5f, -0.5f } },

            // Top face (Y = +0.5)
            { { -0.5f,  0.5f,  0.5f } },
            { {  0.5f,  0.5f,  0.5f } },
            { {  0.5f,  0.5f, -0.5f } },
            { {  0.5f,  0.5f, -0.5f } },
            { { -0.5f,  0.5f, -0.5f } },
            { { -0.5f,  0.5f,  0.5f } },

            // Bottom face (Y = -0.5)
            { { -0.5f, -0.5f, -0.5f } },
            { {  0.5f, -0.5f, -0.5f } },
            { {  0.5f, -0.5f,  0.5f } },
            { {  0.5f, -0.5f,  0.5f } },
            { { -0.5f, -0.5f,  0.5f } },
            { { -0.5f, -0.5f, -0.5f } }
        };

        m_VertexCount = sizeof(cubeVertices) / sizeof(Vertex);

        auto& device = Application::Get().GetContext().GetDevice();

        RHI::BufferDesc desc;
        desc.Size = sizeof(cubeVertices);
        desc.Usage = RHI::BufferUsage::Vertex;
        desc.DebugName = "CubeVB";

        m_VertexBuffer = device.CreateBuffer(desc, std::as_bytes(std::span(cubeVertices)));

        m_CameraBuffers.resize(3);
    }

    void MainLayer::OnDetach()
    {
        OPAL_INFO("Client", "MainLayer::OnDetach()");
        m_CameraBuffers.clear();
        m_CameraBufferCursor = 0;
        m_HasCameraData = false;
        m_VertexBuffer.reset();
        m_Scene.reset();
    }

    void MainLayer::OnEvent(Event& event)
    {

    }

    void MainLayer::OnUpdate(float dt)
    {
        if (m_Scene)
        {
            m_Scene->OnUpdate(dt);
        }
    }

    void MainLayer::OnRender(RenderGraph& graph)
    {
        struct ScenePassData
        {
            RGResourceHandle Output;
            RHI::IPipeline* Pipeline;
        };

        // Create transient Depth Buffer resource for 3D scene rendering
        int width, height;
        Application::Get().GetWindow().GetFramebufferSize(&width, &height);


        RHI::TextureDesc depthDesc;
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.PixelFormat = RHI::Format::D32_FLOAT;
        depthDesc.Usage = RHI::TextureUsage::DepthStencilAttachment;
        depthDesc.DebugName = "SceneDepthBuffer";

        RGResourceHandle depthResource = graph.CreateResource("SceneDepth", depthDesc);

        graph.AddPass<ScenePassData>("GBufferPass",
            [&](RenderGraphBuilder& builder, ScenePassData& data)
            {
                RGResourceHandle backbuffer = graph.GetResource("Backbuffer");
                RGAttachmentInfo colorInfo;
                colorInfo.Handle = backbuffer;
                colorInfo.LoadOp = RHI::LoadOp::Clear;
                colorInfo.ClearColor[0] = 0.05f;
                colorInfo.ClearColor[1] = 0.05f;
                colorInfo.ClearColor[2] = 0.05f;
                colorInfo.ClearColor[3] = 1.0f;

                RGAttachmentInfo depthInfo;
                depthInfo.Handle = depthResource;
                depthInfo.LoadOp = RHI::LoadOp::Clear;
                depthInfo.StoreOp = RHI::StoreOp::DontCare;
                depthInfo.DepthClearValue = 1.0f;

                data.Output = builder.Write(colorInfo);
                builder.Write(depthInfo);

                RHI::PipelineDesc desc;
                desc.VertexShader = builder.LoadShader("Default.slang", RHI::ShaderStage::Vertex);
                desc.FragmentShader = builder.LoadShader("Default.slang", RHI::ShaderStage::Fragment);
                desc.Rasterizer.cullMode = RHI::CullMode::Back;
                desc.DepthStencil.depthTest = true;
                desc.DepthStencil.depthWrite = true;
                desc.DepthStencil.depthCompareOp = RHI::CompareOp::Less;
                desc.DepthAttachmentFormat = RHI::Format::D32_FLOAT;
                data.Pipeline = builder.CreatePipeline(desc);
            },
            [&](const RenderGraphRegistry&, const ScenePassData& data, RHI::ICommandList* cmd)
            {
                if (!data.Pipeline || !m_VertexBuffer || m_VertexCount == 0)
                {
                    return;
                }

                cmd->BindPipeline(data.Pipeline);
                cmd->BindVertexBuffer(m_VertexBuffer.get(), 0);

                if (m_Scene)
                {
                    const auto& window = Application::Get().GetWindow();
                    float aspect = (window.GetHeight() > 0) ? (static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight())) : 1.778f;

                    // Compute ViewProjection matrix from active camera in scene
                    glm::mat4 viewMatrix(1.0f);
                    glm::mat4 projectionMatrix(1.0f);
                    bool foundCamera = false;

                    m_Scene->Each([&](flecs::entity e, const CameraComponent& camera, const TransformComponent& transform) {
                        if (camera.Primary && !foundCamera)
                        {
                            viewMatrix = glm::inverse(transform.GetTransform());
                            float camAspect = camera.FixedAspectRatio ? camera.AspectRatio : aspect;
                            projectionMatrix = glm::perspective(glm::radians(camera.Fov), camAspect, camera.NearClip, camera.FarClip);
                            foundCamera = true;
                        }
                    });

                    if (!foundCamera)
                    {
                        viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                        projectionMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
                    }

                    CameraData camData;
                    camData.ViewProjection = projectionMatrix * viewMatrix;

                    if (m_CameraBuffers.empty()) return;
                    auto& device = Application::Get().GetContext().GetDevice();
                    const bool cameraChanged = !m_HasCameraData
                        || std::memcmp(&m_LastCameraViewProjection, &camData.ViewProjection, sizeof(camData.ViewProjection)) != 0;
                    if (cameraChanged)
                    {
                        const uint32_t camIndex = m_CameraBufferCursor % static_cast<uint32_t>(m_CameraBuffers.size());

                        RHI::BufferDesc camDesc;
                        camDesc.Size = sizeof(CameraData);
                        camDesc.Usage = RHI::BufferUsage::Uniform;
                        camDesc.DebugName = "CameraUBO";

                        Ref<RHI::IBuffer> cameraBuffer = device.CreateBuffer(camDesc, std::as_bytes(std::span(&camData, 1)));
                        if (cameraBuffer)
                        {
                            m_CameraBuffers[camIndex] = std::move(cameraBuffer);
                            m_CameraBufferCursor = (camIndex + 1) % static_cast<uint32_t>(m_CameraBuffers.size());
                            m_LastCameraViewProjection = camData.ViewProjection;
                            m_HasCameraData = true;
                        }
                    }

                    const uint32_t activeCameraIndex = (m_CameraBufferCursor + m_CameraBuffers.size() - 1) % m_CameraBuffers.size();
                    RHI::IBuffer* cameraBuffer = m_CameraBuffers[activeCameraIndex].get();
                    if (!cameraBuffer) return;
                    cmd->SetUniformBuffer(0, cameraBuffer, 0);

                    // Render all active entities with MeshRendererComponent
                    m_Scene->Each([&](flecs::entity, MeshRendererComponent& meshRenderer, const TransformComponent& transform) {
                        if (!meshRenderer.Enabled || !meshRenderer.MaterialAsset)
                        {
                            return;
                        }

                        const uint32_t frameIndex = Application::Get().GetContext().GetCurrentFrameIndex();
                        RHI::IBuffer* materialBuffer = meshRenderer.MaterialAsset->GetUniformBuffer(device, frameIndex);
                        if (!materialBuffer) return;

                        cmd->SetUniformBuffer(0, materialBuffer, 1);
                        glm::mat4 modelMatrix = transform.GetTransform();
                        cmd->PushConstants(data.Pipeline, RHI::ShaderStage::Vertex, &modelMatrix, sizeof(modelMatrix));
                        cmd->Draw(m_VertexCount, 1, 0, 0);
                    });
                }
            }
        );
    }
}
