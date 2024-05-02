# C++ Project Starter Template

This is a little quick-start project template for C++ projects which utilise a Core/App project architecture. There are two included projects - one called _Core_, and one called _App_. [Premake](https://github.com/premake/premake-core) is used to generate project files.

Core builds into a static library and is meant to contain common code intended for use in multiple applications. App builds into an executable and links the Core static library, as well as provides an include path to Core's code.

The `Scripts/` directory contains build scripts for Windows, Linux and MacOSX, and the `vendor/` directory will contain premake and possibly other dependencies.

## Getting Started
1. Clone this repository or use the "Use this template" button on GitHub to quickly set up your own repository based on this template
2. `App/` and `Core/` are the two projects - you can edit the names of these folders and their contents to suit
3. The four included Premake build files are `premake5.lua`, `Dependencies.lua`, `Core/premake5.lua` and `App/premake5.lua` - you can edit these to customise your build configurations, edit the names of your projects and workspace/solution, etc.
4. Open the `Scripts/` directory and then the appropriate folder for your OS. Run the `Setup` file to generate the project files. You can edit the setup scripts to change the type of project that is generated - out of the box they are set to Visual Studio 2022 for Windows and gmake2 for Linux and MacOSX.

## Included
- Some example code (in `App/src` and `Core/src`) to provide a starting point and test
- Simple `.gitignore` to ignore project files and binaries
- Premake binaries for Win/Mac/Linux (`v5.0-beta2`)

## License
- UNLICENSE for this repository (see `UNLICENSE.txt` for more details)
- Premake is licensed under BSD 3-Clause (see included LICENSE.txt file for more details)