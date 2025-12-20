# Mixture

![Build Status](https://img.shields.io/github/actions/workflow/status/YOUR_USERNAME/Mixture/ci.yml?branch=main&label=Build&logo=github)
![License](https://img.shields.io/badge/License-MIT-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)

**Mixture** is a high-performance, modular graphics engine built on **Vulkan**. It features a modern **Render Graph** architecture designed to decouple logical frame construction from the low-level API execution, enabling complex dependency management and automatic barrier insertion.

---

## ✨ Key Features

### 🎨 Rendering Core
- **Vulkan Backend** – Modern bindless design (in progress) with dynamic state management.
- **Render Graph Architecture** – Automatic resource lifetime analysis, dependency sorting, and barrier injection.
- **RHI Abstraction** – Clean separation between logical resources (Front-end) and hardware handles (Back-end).

### 🛠️ Engine Infrastructure
- **Layer Stack System** – Modular update loops (Input -> Game -> UI) inspired by modern game engine architectures.
- **Cross-Platform** – First-class support for Windows and Linux.
- **Developer Tools** – Integrated rich logging (spdlog), profiling, and validation layers.
- **Robust Testing** – Unit testing suite via **Google Test**.

---

## 🔧 Prerequisites

Before building the engine, ensure you have the following installed:

1.  **Vulkan SDK** (1.3+) – [Download Here](https://vulkan.lunarg.com/)
    * *Note: Ensure `VULKAN_SDK` is in your environment variables.*
2.  **C++ Compiler** – MSVC (Windows) or GCC/Clang (Linux/macOS) supporting C++17/20.
3.  **Git** – For cloning the repository.

---

## 📦 Getting Started

### 1. Clone the Repository
Clone recursively to ensure vendor submodules (like Google Test) are pulled.
```bash
git clone --recursive [https://github.com/YOUR_USERNAME/Mixture.git](https://github.com/YOUR_USERNAME/Mixture.git)
```

### 2. Setup Dependencies

Run the platform-specific setup script to download Premake and generate project files.
If executed in VS Code this will also generate '.vscode/c_cpp_properties.json' for syntax highlighting.

**Windows**:
```PowerShell
./Scripts/Setup-Windows.ps1
```

**Unix**:
```bash
sh ./Scripts/Setup-Unix.sh
```

### 3. Build & Run

Open the generated solution file (e.g., Mixture.sln on Windows or Makefile on Linux) in your IDE.
- Start Project: Select Sandbox or App as your startup project.
- Build Config: Recommended Debug for development (enables Validation Layers).

#### 🧪 Running Tests
Mixture uses Google Test to ensure the stability of the Render Graph and core logic.
1. Build the Tests project configuration.
2. Run the executable located in bin/Debug-platform-arch/Tests/.
    ```Bash
    # Example CLI run
    ./bin/Debug-windows-x86_64/Tests/Tests.exe
    ```

#### 📚 Documentation
Internal API documentation is generated using Doxygen.
1. Ensure Doxygen is installed.
2. Run the generation command in the root directory:
    ```Bash
    doxygen Doxyfile
    ```

3. Open docs/html/index.html to view the API reference.

---

## 🏗️ Architecture Overview
The engine is structured into three primary layers:
1. The Frontend (Render Graph):
    - Defines Passes, Resources, and Dependencies.
    - API-Agnostic. Pure C++ logic.

2. The RHI (Render Hardware Interface):
    - Abstract interfaces (IGraphicsDevice, IBuffer, ITexture).
    - Defines the contract for the backend.

3. The Backend (Vulkan):
    - Concrete implementation using vulkan.hpp.
    - Manages vk::Device, Memory Allocators (VMA), and Command Buffers.
    - Copyright © 2025. All rights reserved.
