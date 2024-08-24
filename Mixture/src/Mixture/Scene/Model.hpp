#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Texture.hpp"
#include "Mixture/Renderer/Buffer/IndexBuffer.hpp"
#include "Mixture/Renderer/Buffer/VertexBuffer.hpp"

#include <glm/glm.hpp>

namespace Mixture
{
    class Model
    {
    public:
        struct Builder
        {
            std::vector<Vertex> Vertices{};
            std::vector<uint32_t> Indices{};

            void LoadModel(const std::string& modelName);
        };

        Model(Builder& builder);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        void Bind(CommandBuffer commandBuffer);
        void Draw(CommandBuffer commandBuffer);

        static Scope<Model> Create(const std::string& modelName);
        
    private:
        Scope<VertexBuffer> m_VertexBuffer;
        Scope<IndexBuffer> m_IndexBuffer;
    };
}
