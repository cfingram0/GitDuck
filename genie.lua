
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
        links {"DuckGfx", "Core", "d3d11", "dxgi", "d3dcompiler", "assimp"}
        includedirs {"Core/include", "DuckGfx/include", "External/Assimp/include"}
        defines {"WIN32"}
        flags {"WinMain", "Symbols"}
        
        configuration {"Debug", "x64"}
            targetdir ".build/bin/debug/x64"
            debugdir ".build/bin/debug/x64"
            libdirs {"External/Assimp/lib/x64"}
        configuration {"Debug", "x32"}
            targetdir ".build/bin/debug/x86"
            debugdir ".build/bin/debug/x86"
            libdirs {"External/Assimp/lib/x86"}
        configuration {"Release", "x64"}
            targetdir ".build/bin/release/x64"
            debugdir ".build/bin/release/x64"
            libdirs {"External/Assimp/lib/x64"}
        configuration {"Release", "x32"}
            targetdir ".build/bin/release/x86"
            debugdir ".build/bin/release/x86"
            libdirs {"External/Assimp/lib/x86"}
            
    project "DuckGfx"
        kind "StaticLib"
        files {"DuckGfx/src/*.*", "DuckGfx/include/*.*", "DuckGfx/shaders/pixel/*.*", "DuckGfx/shaders/vertex/*.*"}
        includedirs {"Core/include", "DuckGfx/include"}
        flags {"Symbols"}

        configuration {"Debug", "x64"}
            targetdir ".build/bin/debug/x64"
            debugdir ".build/bin/debug/x64"
        configuration {"Debug", "x32"}
            targetdir ".build/bin/debug/x86"
            debugdir ".build/bin/debug/x86"
        configuration {"Release", "x64"}
            targetdir ".build/bin/release/x64"
            debugdir ".build/bin/release/x64"
        configuration {"Release", "x32"}
            targetdir ".build/bin/release/x86"
            debugdir ".build/bin/release/x86"
        
     project "Core"
        kind "StaticLib"
        files {"Core/src/*.*", "Core/include/*.*"}
        includedirs {"Core/include"}
        flags {"Symbols"}
        
        configuration {"Debug", "x64"}
            targetdir ".build/bin/debug/x64"
            debugdir ".build/bin/debug/x64"
        configuration {"Debug", "x32"}
            targetdir ".build/bin/debug/x86"
            debugdir ".build/bin/debug/x86"
        configuration {"Release", "x64"}
            targetdir ".build/bin/release/x64"
            debugdir ".build/bin/release/x64"
        configuration {"Release", "x32"}
            targetdir ".build/bin/release/x86"
            debugdir ".build/bin/release/x86"