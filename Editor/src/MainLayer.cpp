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

        // Create 3D Cube primitive vertex buffer (36 vertices)
        Vertex cubeVertices[] = {
            // Front face
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            // Back face
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            // Top face
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            // Bottom face
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            // Right face
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            // Left face
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } }
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

        graph.AddPass<ScenePassData>("GBufferPass",
            [&](RenderGraphBuilder& builder, ScenePassData& data)
            {
                if (m_VertexBuffer)
                {
                    graph.ImportResource("SceneVB", m_VertexBuffer.get());
                }

                RGResourceHandle backbuffer = graph.GetResource("Backbuffer");
                RGAttachmentInfo info;
                info.Handle = backbuffer;
                info.LoadOp = RHI::LoadOp::Clear;
                info.ClearColor[0] = 0.05f;
                info.ClearColor[1] = 0.05f;
                info.ClearColor[2] = 0.05f;
                info.ClearColor[3] = 1.0f;

                data.Output = builder.Write(info);

                RHI::PipelineDesc desc;
                desc.VertexShader = builder.LoadShader("Default.slang", RHI::ShaderStage::Vertex);
                desc.FragmentShader = builder.LoadShader("Default.slang", RHI::ShaderStage::Fragment);
                desc.Rasterizer.cullMode = RHI::CullMode::None;
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
                    // Compute ViewProjection matrix from active camera in scene
                    glm::mat4 viewMatrix(1.0f);
                    glm::mat4 projectionMatrix(1.0f);
                    bool foundCamera = false;

                    m_Scene->Each([&](flecs::entity e, const CameraComponent& camera, const TransformComponent& transform) {
                        if (camera.Primary && !foundCamera)
                        {
                            viewMatrix = glm::inverse(transform.GetTransform());
                            float aspect = camera.FixedAspectRatio ? camera.AspectRatio : (1280.0f / 720.0f);
                            projectionMatrix = glm::perspective(glm::radians(camera.Fov), aspect, camera.NearClip, camera.FarClip);
                            foundCamera = true;
                        }
                    });

                    if (!foundCamera)
                    {
                        viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                        projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
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
