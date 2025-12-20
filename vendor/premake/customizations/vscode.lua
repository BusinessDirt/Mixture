-- scripts/vscode_gen.lua

newaction {
    trigger = "vscode",
    description = "Generate VS Code c_cpp_properties.json (Merged)",
    execute = function()
        local function serialize_array(arr)
            local res = ""
            for i, v in ipairs(arr) do
                -- Escape backslashes for JSON
                local safe_path = v:gsub("\\", "\\\\")
                res = res .. '\t\t\t\t"' .. safe_path .. '"'
                if i < #arr then res = res .. ",\n" end
            end
            return res
        end

        -- Helper to remove duplicates from a table
        local function table_unique(t)
            local seen = {}
            local res = {}
            for _, v in ipairs(t) do
                if not seen[v] then
                    seen[v] = true
                    table.insert(res, v)
                end
            end
            return res
        end

        if not os.isdir(".vscode") then
            os.mkdir(".vscode")
        end

        -- GLOBAL ACCUMULATORS
        local global_includes = {}
        local global_defines = {}
        local seen_projects = {}

        -- Defaults
        local final_c_std = "c17"
        local final_cpp_std = "c++17"

        -- 1. Get Iterator
        local each_wks = premake.global.eachWorkspace or premake.global.eachSolution
        if not each_wks then
            print("Error: Could not locate premake.global.eachWorkspace")
            return
        end

        -- 2. Loop EVERYTHING
        for wks in each_wks() do
            local projects = wks.projects or {}

            for _, prj in ipairs(projects) do
                if not seen_projects[prj.name] then
                    seen_projects[prj.name] = true

                    print("Merging project: " .. prj.name)

                    local each_cfg = premake.project.eachConfig or premake.project.eachconfig
                    if each_cfg then
                        for cfg in each_cfg(prj) do

                            -- A. Collect Include Dirs
                            if cfg.includedirs then
                                for _, inc in ipairs(cfg.includedirs) do
                                    local abs_path = path.getabsolute(inc)
                                    table.insert(global_includes, abs_path)
                                end
                            end

                            -- B. Collect System Include Dirs
                            if cfg.externalincludedirs then
                                for _, inc in ipairs(cfg.externalincludedirs) do
                                    local abs_path = path.getabsolute(inc)
                                    table.insert(global_includes, abs_path)
                                end
                            end

                            -- C. Collect Defines
                            if cfg.defines then
                                for _, def in ipairs(cfg.defines) do
                                    table.insert(global_defines, def)
                                end
                            end

                            -- D. Update Standards (Take the highest/latest found)
                            -- This is simple overwrite logic; usually sufficient.
                            if cfg.cdialect then final_c_std = cfg.cdialect:lower() end
                            if cfg.cppdialect then
                                local std = cfg.cppdialect:lower()
                                if std:find("gnu") then std = std:gsub("gnu", "c") end
                                final_cpp_std = std
                            end

                            -- Only grab the first config (Debug) per project to avoid path duplication madness
                            break
                        end
                    end
                end
            end
        end

        -- 3. Deduplicate everything
        global_includes = table_unique(global_includes)
        global_defines = table_unique(global_defines)

        -- 4. Generate the Single Monolithic Config
        local config_entry = string.format([[
        {
            "name": "Mixture",
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
]], serialize_array(global_includes), serialize_array(global_defines), final_c_std, final_cpp_std)

        local file_content = string.format([[
{
    "configurations": [
%s
    ],
    "version": 4
}
]], config_entry)

        local f = io.open(".vscode/c_cpp_properties.json", "w+")
        if f then
            f:write(file_content)
            f:close()
            print("Generated .vscode/c_cpp_properties.json with merged paths.")
        else
            print("Error: Could not write to .vscode/c_cpp_properties.json")
        end
    end
}
