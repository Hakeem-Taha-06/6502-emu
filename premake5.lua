workspace "6502emu"
    configurations{
        "Debug", 
        "Release"
    }
    architecture "x64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "6502emu"
    location "6502emu"
    language "C++"
    kind "ConsoleApp"

    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-int/"..outputdir.."/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h",
        "%{prj.name}/dependencies/src/**.cpp",
        "%{prj.name}/dependencies/src/**.c"
    }

    includedirs
    {
        "%{wks.location}/%{prj.name}/dependencies/include",
        "%{wks.location}/%{prj.name}/dependencies/include/imgui",
        "%{prj.name}/src",
        "%{prj.name}/include"
    }

    libdirs
    {
        "%{wks.location}/%{prj.name}/dependencies/libs"
    }
    
    links 
    {
        "glfw3",
        "opengl32"
    }

    cppdialect "C++17"
    staticruntime "Off"
    systemversion "latest"

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "on"
        linkoptions { "/IGNORE:4099" }

    filter "configurations:Release"
        defines "RELEASE"
        symbols "off"
        optimize "on"
