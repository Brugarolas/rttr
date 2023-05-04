project "rttr"
    kind "StaticLib"
    language "C++"
    staticruntime "off"
    
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/rttr/**.h",
        "src/rttr/**.h.in",
        "src/rttr/**.cpp"
    }

    includedirs
    {
        "%{IncludeDirs.rttr}"
    }

    defines 
    {
    }

    filter "system:windows"
        systemversion "latest"
    
    filter "configurations:Debug"
        defines "NIT_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:EditorRelease"
        defines "NIT_RELEASE"
        runtime "Release"
        optimize "on"
        
    filter "configurations:GameRelease"
          defines "NIT_RELEASE"
          runtime "Release"
          optimize "on"
