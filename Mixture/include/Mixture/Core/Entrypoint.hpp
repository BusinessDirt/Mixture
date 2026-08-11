#pragma once

/**
 * @file Entrypoint.hpp
 * @brief Entry point for the application, handling platform-specific main functions.
 */

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
    builder.UseConsoleSink().UseFileSink("latest.log");

    Opal::LogRegistry::Get().SetThreadName("Main Thread");
    Opal::LogRegistry::Get().Initialize(builder.Build());
    
    const Mixture::Application* app = Mixture::CreateApplication({.Count = argc, .Args = argv });
    app->Run();
    delete app;

    return 0;
}

#if defined(OPAL_DIST) && defined(OPAL_PLATFORM_WINDOWS)

// ---------------------------------------------------------
// WINDOWS DISTRIBUTION ENTRY POINT (Hides the console)
// ---------------------------------------------------------
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    // __argc and __argv are MSVC compiler extensions that fetch
    // the command line arguments without needing standard main()
    return Entrypoint(__argc, __argv);
}

#else

// ---------------------------------------------------------
// STANDARD ENTRY POINT
// (Used for all Mac/Linux builds, and Windows Debug builds)
// ---------------------------------------------------------
int main(const int argc, char** argv)
{
    return Entrypoint(argc, argv);
}

#endif // OPAL_DIST && OPAL_PLATFORM_WINDOWS
