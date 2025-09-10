#include "MainLayer.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Mixture
{
    void MainLayer::OnAttach()
    {
        const std::vector<Vertex> vertices = {
            {.Position = { -1.0f, -1.0f, 0.0f }, .Color = { 1.0f, 1.0f, 1.0f, 1.0f }, .UV = { 0.0f, 0.0f } },
            {.Position = {  1.0f, -1.0f, 0.0f }, .Color = { 1.0f, 1.0f, 1.0f, 1.0f }, .UV = { 1.0f, 0.0f } },
            {.Position = {  1.0f,  1.0f, 0.0f }, .Color = { 1.0f, 1.0f, 1.0f, 1.0f }, .UV = { 1.0f, 1.0f } },
            {.Position = { -1.0f,  1.0f, 0.0f }, .Color = { 1.0f, 1.0f, 1.0f, 1.0f }, .UV = { 0.0f, 1.0f } }
        };

        const std::vector<uint32_t> indices = {
            0, 1, 2, 0, 2, 3
        };
        
        m_GraphicsPipeline = GraphicsPipeline::Create("MixtureBuiltin.Material");
        m_VertexBuffer = VertexBuffer::Create(vertices.data(), sizeof(Vertex), vertices.size());
        m_IndexBuffer = IndexBuffer::Create(indices);
        
        m_UniformBufferObject = {};
        m_UniformBuffer = UniformBuffer::Create(sizeof(UniformBufferObject));

        m_Texture = Application::Get().GetAssetManager().LoadTexture<Texture2D>("test.png");
    }

    void MainLayer::OnDetach()
    {
        
    }

    void MainLayer::OnUpdate(FrameInfo& frameInfo)
    {
        m_UniformBufferObject.View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -m_Z));
        m_UniformBufferObject.Projection = glm::perspective(glm::radians(45.0f), Application::Get().GetWindow().GetAspectRatio(), 0.1f, 100.0f);
        
        m_UniformBuffer->SetData(&m_UniformBufferObject);
        m_GraphicsPipeline->UpdateGlobalUniformBuffer(m_UniformBuffer->GetDescriptorInfo());
        m_GraphicsPipeline->UpdateTexture(m_Texture);
    }

    void MainLayer::OnRender(FrameInfo& frameInfo)
    {
        m_GraphicsPipeline->Bind();
        m_VertexBuffer->Bind();
        m_IndexBuffer->Bind();
        
        RenderCommand::DrawIndexed(m_IndexBuffer);
    }

    void MainLayer::OnRenderImGui(FrameInfo& frameInfo)
    {
        ImGuiWidgets::Dockspace("MainDockspace", true, ImGuiWindowFlags_NoBackground, ImGuiDockNodeFlags_PassthruCentralNode);
        
        ImGui::Begin("Test");
        ImGui::Text("Frame time: %.3f ms", frameInfo.FrameTime);
        ImGui::Text("Viewport focused: %s", m_ViewportFocused ? "true" : "false");
        ImGui::SliderFloat("Distance", &m_Z, 0.1f, 100.0f);
        ImGui::End();
    }

    void MainLayer::OnEvent(Event& event)
    {
        
    }
}
