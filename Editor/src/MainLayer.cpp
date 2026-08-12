#include "MainLayer.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/Scene/Components.hpp"
#include "Mixture/Scene/Entity.hpp"

namespace Mixture
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    struct PushConstantData
    {
        glm::mat4 Model;
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
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },

            // Right face (X = +0.5)
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },

            // Back face (Z = -0.5)
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },

            // Left face (X = -0.5)
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },

            // Top face (Y = +0.5)
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },

            // Bottom face (Y = -0.5)
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } }
        };

        m_VertexCount = sizeof(cubeVertices) / sizeof(Vertex);

        RHI::BufferDesc desc;
        desc.Size = sizeof(cubeVertices);
        desc.Usage = RHI::BufferUsage::Vertex;
        desc.DebugName = "CubeVB";

        m_VertexBuffer = Application::Get().GetContext().GetDevice().CreateBuffer(desc, std::as_bytes(std::span(cubeVertices)));
    }

    void MainLayer::OnDetach()
    {
        OPAL_INFO("Client", "MainLayer::OnDetach()");
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
        const auto& window = Application::Get().GetWindow();
        uint32_t width = window.GetWidth() > 0 ? window.GetWidth() : 1280;
        uint32_t height = window.GetHeight() > 0 ? window.GetHeight() : 720;

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
                if (m_VertexBuffer)
                {
                    graph.ImportResource("SceneVB", m_VertexBuffer.get());
                }

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
            [&](const RenderGraphRegistry& registry, const ScenePassData& data, RHI::ICommandList* cmd)
            {
                cmd->BindPipeline(data.Pipeline);
                if (m_VertexBuffer)
                {
                    cmd->BindVertexBuffer(m_VertexBuffer.get(), 0);
                }

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

                    glm::mat4 viewProjection = projectionMatrix * viewMatrix;

                    // Render all active entities with MeshRendererComponent
                    m_Scene->Each([&](flecs::entity e, const MeshRendererComponent& meshRenderer, const TransformComponent& transform) {
                        if (meshRenderer.Enabled)
                        {
                            PushConstantData pushData;
                            pushData.Model = transform.GetTransform();
                            pushData.ViewProjection = viewProjection;

                            cmd->PushConstants(data.Pipeline, RHI::ShaderStage::Vertex, &pushData, sizeof(PushConstantData));
                            cmd->Draw(m_VertexCount, 1, 0, 0);
                        }
                    });
                }
            }
        );
    }
}
