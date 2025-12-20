-- scripts/vscode_gen.lua

newaction {
    trigger = "vscode",
    description = "Generate VS Code c_cpp_properties.json",
    execute = function()
        local function serialize_array(arr)
            local res = ""
            for i, v in ipairs(arr) do
                res = res .. '\t\t\t\t"' .. v .. '"'
                if i < #arr then res = res .. ",\n" end
            end
            return res
        end

        if not os.isdir(".vscode") then
            os.mkdir(".vscode")
        end

        local cfg_container = {}
        local seen_names = {}

        -- 1. Get the Workspace Iterator
        local each_wks = premake.global.eachWorkspace or premake.global.eachSolution

        if not each_wks then
            print("Error: Could not locate premake.global.eachWorkspace")
            return
        end

        -- 2. Iterate Workspaces
        for wks in each_wks() do

            -- 3. Iterate Projects directly from the table
            local projects = wks.projects or {}

            for _, prj in ipairs(projects) do
                if not seen_names[prj.name] then
                    seen_names[prj.name] = true

                    -- 4. Iterate Configurations
                    local each_cfg = premake.project.eachConfig or premake.project.eachconfig

                    if each_cfg then
                        for cfg in each_cfg(prj) do

                            -- Build Include Paths
                            local include_paths = {}
                            if cfg.includedirs then
                                for _, inc in ipairs(cfg.includedirs) do
                                    local abs_path = path.getabsolute(inc)
                                    table.insert(include_paths, abs_path)
                                end
                            end

                            -- Build Defines
                            local defines = {}
                            if cfg.defines then
                                for _, def in ipairs(cfg.defines) do
                                    table.insert(defines, def)
                                end
                            end

                            -- DETECT C/C++ VERSIONS
                            -- Default to safe modern standards if not specified
                            local c_std = "c17"
                            local cpp_std = "c++17"

                            if cfg.cdialect then
                                -- Convert "C11" -> "c11"
                                c_std = cfg.cdialect:lower()
                            end

                            if cfg.cppdialect then
                                -- Convert "C++20" -> "c++20"
                                -- Also handle "gnu++17" -> "c++17" if necessary for VS Code strictness,
                                -- though VS Code often accepts gnu++ modes now.
                                cpp_std = cfg.cppdialect:lower()

                                -- Simple fix for "gnu" dialects if VS Code complains (optional safety)
                                if cpp_std:find("gnu") then
                                    cpp_std = cpp_std:gsub("gnu", "c")
                                end
                            end

                            local config_entry = string.format([[
        {
            "name": "%s",
            "includePath": [
%s
            ],
            "defines": [
%s
            ],
            "macFrameworkPath": [
                "/System/Library/Frameworks",
                "/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang",
            "cStandard": "%s",
            "cppStandard": "%s",
            "intelliSenseMode": "macos-clang-x64"
        }
]], prj.name, serialize_array(include_paths), serialize_array(defines), c_std, cpp_std)

                            table.insert(cfg_container, config_entry)

                            -- Only grab the first config (Debug)
                            break
                        end
                    end
                end
            end
        end

        local file_content = string.format([[
{
    "configurations": [
%s
    ],
    "version": 4
}
]], table.concat(cfg_container, ","))

        local f = io.open(".vscode/c_cpp_properties.json", "w+")
        if f then
            f:write(file_content)
            f:close()
            print("Generated .vscode/c_cpp_properties.json")
        else
            print("Error: Could not write to .vscode/c_cpp_properties.json")
        end
    end
}
