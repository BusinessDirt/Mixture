#include "mxpch.hpp"
#include "MainLayer.hpp"

namespace Mixture
{
    struct Push
    {
        glm::mat4 ModelMatrix;
        glm::mat4 NormalMatrix;
    };

    void MainLayer::OnAttach()
    {
        m_Pipeline = GraphicsPipeline::Create("shader");
        
        //const ShaderInformation& vertInfo = Application::Get().GetAssetManager().GetShader("shader.vert").GetInformation();
        //const ShaderInformation& fragInfo = Application::Get().GetAssetManager().GetShader("shader.frag").GetInformation();
        
        // TODO: move this out of here and also create the descriptor set from the texture sampler there
        // TODO: update set when "binding" texture
        // TODO: the group models by their texture and material etc to save update time
        m_GlobalUbo = UniformBuffer::CreateGlobal();
        m_Texture = Texture::Create("viking_room.png");

        m_Camera = EditorCamera(90.0f, Application::Get().GetWindow().GetAspectRatio(), 0.01f, 100.0f);
        
        m_Model = Model::Create("viking_room.obj");
    }

    void MainLayer::OnDetach()
    {
        m_Model = nullptr;
        m_Texture = nullptr;
        m_GlobalUbo = nullptr;
        m_Pipeline = nullptr;
    }

    void MainLayer::OnUpdate(FrameInfo& frameInfo)
    {
        m_FrameTime = frameInfo.FrameTime;
        
        m_Camera.OnUpdate(frameInfo.FrameTime);
        
        GlobalUniformBuffer ubo{};
        ubo.ViewMatrix = m_Camera.GetViewMatrix();
        ubo.ProjectionMatrix = m_Camera.GetProjection();
        m_GlobalUbo->Update(&ubo);
        
        m_Pipeline->Bind(frameInfo);
        
        Push push{};
        push.ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        push.NormalMatrix = glm::transpose(glm::inverse(glm::mat3(push.ModelMatrix)));
        m_Pipeline->PushConstants(frameInfo, &push);
        
        // TODO: ECS
        m_Texture->Bind();
        m_Model->Bind(frameInfo.CommandBuffer);
        m_Model->Draw(frameInfo.CommandBuffer);
    }

    void MainLayer::OnUIRender()
    {
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

        ImGuiUtil::DockSpace("DockSpaceDemo", nullptr, dockspaceFlags);
        //ImVec2 viewport = ImGuiUtil::Viewport();
        
        ImGui::Begin("Settings");
        ImGui::Text("FPS: %f", (1.0f / m_FrameTime));
        //ImGui::Text("Viewport Size: %dx%d", (int)viewport.x, (int)viewport.y);
        ImGui::End();
    }

    void MainLayer::OnEvent(Event& event)
    {
        m_Camera.OnEvent(event);
    }
}
