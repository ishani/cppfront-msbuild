-- ----------------------------------------------------------------------------------------------------------------------
-- sort out where we're building

Paths                = {}
Paths.GenerationRoot = "_gen"
Paths.Initial        = os.getcwd()
Paths.InputExt       = path.join("..", "ext")
Paths.InputSrc       = path.join("..", "src")

print("Premake launch directory: " .. Paths.Initial)
os.mkdir(Paths.GenerationRoot)

-- ----------------------------------------------------------------------------------------------------------------------
-- check if cppfront.exe exists in _gen; if not, we build it from scratch
-- do to that, we need vswhere.exe to be able to spin up a build environment - so grab that from the ms repo if it's not in _gen

local cppfrontFileLocalPath, err = os.realpath(path.join(Paths.GenerationRoot, "cppfront.exe"))
local vswhereFileLocalPath, err = os.realpath(path.join(Paths.GenerationRoot, "vswhere.exe"))

print("Checking for cppfront : " .. cppfrontFileLocalPath)
if (os.isfile(cppfrontFileLocalPath) == false) then
    print("cppfront missing, building from source ... ")
    print("Checking for vswhere : " .. vswhereFileLocalPath)

    if (os.isfile(vswhereFileLocalPath) == false) then
        print("Downloading vswhere from github ... ")
        local result_str, response_code = http.download(
            "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe",
            vswhereFileLocalPath, {})

        if response_code ~= 200 then
            print("Unable to download vswhere (response = " .. response_code .. "), cannot build cppfront")
            return
        end
    end

    local builtCppfrontOk = os.execute("premake.utils.build_cppfront.bat")
    if builtCppfrontOk ~= true then
        print("Failed to build cppfront; provide a pre-built executable in " ..
        Paths.GenerationRoot .. " or fix any reported errors")
        return
    end
end

-- ----------------------------------------------------------------------------------------------------------------------

include "cpp2-premake/premake.lua"


function ConfigureCommonBuildSettings()
    objdir("$(SolutionDir)_obj/%{cfg.shortname}/$(ProjectName)/")
    debugdir("$(OutDir)")
    targetdir("$(SolutionDir)_builds/$(Configuration)/%{cfg.platform}")
end

function ConfigureCommonBuildFiles()
    includedirs {
        Paths.InputSrc,
        path.join(Paths.InputExt, "cppfront", "include"),
    }
    files {
        path.join(Paths.InputExt, "cppfront", "include", "*.h"),
        
        path.join(Paths.InputSrc, "**.cpp"),
        path.join(Paths.InputSrc, "**.cpp2"),
        path.join(Paths.InputSrc, "**.h"),
        path.join(Paths.InputSrc, "**.h2"),
        path.join(Paths.InputSrc, "**.inl"),
    }

    local fmtPath = path.join(Paths.InputExt, "fmt")
    includedirs {
        path.join(fmtPath, "include"),
    }
    files {
        path.join(fmtPath, "src", "format.cc"),
        path.join(fmtPath, "src", "os.cc"),
        path.join(fmtPath, "include", "**.h"),
    }
end

-- ----------------------------------------------------------------------------------------------------------------------
workspace("cpp2_msbuild_" .. _ACTION)

    configurations { "Debug", "Release" }
    platforms { "x86_64" }

    location(Paths.GenerationRoot)

    rules { "cpp2" }

    filter "platforms:x86_64"
    architecture "x64"
    system "windows"
    defines {
        "WIN32",
        "_WINDOWS",
    }
    defines {
        "WIN32_LEAN_AND_MEAN",
        "NOMINMAX",
        "NODRAWTEXT",
        "NOBITMAP",
        "NOMCX",
        "NOSERVICE",
        "NOHELP",
    }
    buildoptions { "/utf-8" }
    cpp2Vars {
        FormatColonErrors    = true,
        EmitCleanCpp1        = true,  -- prefer debugging direct in generated cpp1 files

        StdHandling          = "ImportAll",

        DisableCppEH         = true,

        DispatchToolLocation = "$(SolutionDir)dispatch.exe",
    }

    filter {}

    filter "configurations:Debug"
    defines { "DEBUG", "BUILD_DEBUG=1", "BUILD_RELEASE=0" }
    symbols "On"

    filter "configurations:Release"
    defines { "NDEBUG", "BUILD_DEBUG=0", "BUILD_RELEASE=1" }
    flags { "LinkTimeOptimization" }
    optimize "Full"

    filter {}


-- ----------------------------------------------------------------------------------------------------------------------
project "cpp2_playground"

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    vectorextensions("AVX")
    exceptionhandling("Off")    -- DisableCppEH for cppfront

    conformancemode("true")

    -- MSVC frequently crashes when dealing with cpp2 generated code!
    toolset ("clang")

    -- use modules by default, matching what we ask cppfront for above
    enablemodules("On")
    buildstlmodules("On")

    ConfigureCommonBuildSettings()
    ConfigureCommonBuildFiles()
