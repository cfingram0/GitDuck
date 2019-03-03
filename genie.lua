
solution "DuckSolution"
    location ".build"
    configurations {
        "Debug", 
        "Release"
    }
    
    platforms {
        "x32", 
        "x64"
    }
    
    language "C++"
    
    project "Samples"
        kind "WindowedApp"
        files "DuckGfx_Samples/src/*.*"
        links {"DuckGfx", "Core", "d3d11", "dxgi", "d3dcompiler"}
        includedirs {"Core/include", "DuckGfx/include"}
        defines {"WIN32"}
        flags {"WinMain"}
        
        configuration "Debug"
            targetdir ".build/bin/debug"
        configuration "Release"
            targetdir ".build/bin/release"
        
    project "DuckGfx"
        kind "StaticLib"
        files {"DuckGfx/src/*.*", "DuckGfx/include/*.*", "DuckGfx/shaders/pixel/*.*", "DuckGfx/shaders/vertex/*.*"}
        includedirs {"Core/include", "DuckGfx/include"}

        configuration "Debug"
            targetdir ".build/bin/debug"
        configuration "Release"
            targetdir ".build/bin/release"
        
     project "Core"
        kind "StaticLib"
        files {"Core/src/*.*", "Core/include/*.*"}
        includedirs {"Core/include"}
        
        configuration "Debug"
            targetdir ".build/bin/debug"
        configuration "Release"
            targetdir ".build/bin/release"