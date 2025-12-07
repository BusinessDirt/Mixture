#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/Application.hpp"

extern Mixture::Application* Mixture::CreateApplication(ApplicationCommandLineArgs args);  // NOLINT(readability-redundant-declaration)

/**
 * @brief Entry point function for the application.
 * 
 * Initializes the log registry and runs the application.
 * 
 * @param argc Argument count.
 * @param argv Argument values.
 * @return int Exit code.
 */
inline int Entrypoint(const int argc, char** argv)
{
    Opal::LogBuilder builder;
    builder.UseConsoleSink()
           .UseFileSink("logs/latest.log");

    Opal::LogRegistry::Get().Initialize(builder.Build());

    const Mixture::Application* app = Mixture::CreateApplication({.Count = argc, .Args = argv });
    app->Run();
    delete app;

    return 0;
}

#ifdef OPAL_DIST

#if defined(OPAL_PLATFORM_WINDOWS)

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return Entrypoint(__argc, __argv);
}

#elif defined(OPAL_PLATFORM_MACOSX)



#else
#error OS not supported
#endif // MX_PLATFORM_WINDOWS

#else

int main(const int argc, char** argv)
{
    return Entrypoint(argc, argv);
}

#endif // MX_DIST
