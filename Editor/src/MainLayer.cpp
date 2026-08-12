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
                    // Render all active entities with MeshRendererComponent
                    uint32_t meshCount = 0;
                    m_Scene->Each([&](flecs::entity e, const MeshRendererComponent& meshRenderer, const TransformComponent& transform) {
                        if (meshRenderer.Enabled)
                        {
                            meshCount++;
                        }
                    });

                    if (meshCount > 0)
                    {
                        cmd->Draw(m_VertexCount, meshCount, 0, 0);
                    }
                }
            }
        );
    }
}
