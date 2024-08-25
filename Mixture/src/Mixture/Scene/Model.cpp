#include "mxpch.hpp"
#include "Model.hpp"

#include "Mixture/Renderer/DrawCommand.hpp"
#include "Mixture/Core/Application.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Mixture
{
    Model::Model(Builder& builder)
    {
        m_VertexBuffer = VertexBuffer::Create(builder.Vertices);
        m_IndexBuffer = IndexBuffer::Create(builder.Indices);
    }

    Model::~Model()
    {
        m_IndexBuffer = nullptr;
        m_VertexBuffer = nullptr;
    }

    Scope<Model> Model::Create(const std::string& modelName)
    {
        Builder builder{};
        builder.LoadModel(modelName);

        MX_CORE_INFO("Vertex count ({0}): {1}", modelName, builder.Vertices.size());

        return CreateScope<Model>(builder);
    }

    void Model::Bind(CommandBuffer commandBuffer)
    {
        m_VertexBuffer->Bind(commandBuffer);
        m_IndexBuffer->Bind(commandBuffer);
    }

    void Model::Draw(CommandBuffer commandBuffer)
    {
        DrawCommand::DrawIndexed(commandBuffer, m_IndexBuffer->GetIndexCount());
    }

    void Model::Builder::LoadModel(const std::string& modelName)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::string modelPath = Application::Get().GetAssetManager().GetModelPath(modelName).string();
        bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str());
        MX_CORE_ASSERT(result, warn + err);

        Vertices.clear();
        Indices.clear();
        
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                if (index.vertex_index >= 0)
                {
                    vertex.Position = glm::vec3
                    (
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    );

                    vertex.Color = glm::vec3
                    (
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    );
                }
                
                if (index.normal_index >= 0)
                {
                    vertex.Normal = glm::vec3(
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    );
                }

                if (index.texcoord_index >= 0)
                {
                    vertex.UV = glm::vec2
                    (
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    );
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(Vertices.size());
                    Vertices.push_back(vertex);
                }

                Indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}
