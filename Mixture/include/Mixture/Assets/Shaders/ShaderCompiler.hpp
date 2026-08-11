#pragma once

/**
 * @file ShaderCompiler.hpp
 * @brief Utilities for compiling and reflecting shader code.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/IGraphicsContext.hpp"

namespace Mixture
{
    struct ShaderCompileResult
    {
        Vector<uint8_t> Bytecode;
        std::string Diagnostics;

        bool Succeeded() const { return !Bytecode.empty(); }
    };

    /**
     * @brief Static class for compiling shader source code.
     */
    class ShaderCompiler
    {
    public:
        /** @brief Returns whether the DXC runtime is available for compilation. */
        static bool IsAvailable();

        /**
         * @brief Compiles shader source code into SPIR-V.
         *
         * @param source The shader source code.
         * @return Vector<uint8_t> The compiled SPIR-V bytecode.
         */
        static Vector<uint8_t> Compile(const std::string& source);

        /** @brief Compiles source into bytecode for the requested graphics API. */
        static Vector<uint8_t> Compile(const std::string& source, RHI::GraphicsAPI graphicsAPI);

        /** @brief Compiles source and returns bytecode plus structured diagnostics. */
        static ShaderCompileResult CompileDetailed(const std::string& source);

        /** @brief Compiles source into bytecode for the requested graphics API. */
        static ShaderCompileResult CompileDetailed(const std::string& source, RHI::GraphicsAPI graphicsAPI);
    };
}
