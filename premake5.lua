workspace "YakuzaParless"
	platforms { "Win64" }
	startproject "YakuzaParless"

project "MinHook"
	kind "StaticLib"
	language "C++"
	files { "source/minhook/**" }

project "YakuzaParless"
	kind "SharedLib"
	targetextension ".asi"
	language "C++"
	links { "MinHook" }

	files { "**/MemoryMgr.h", "**/Trampoline.h", "**/Patterns.*", "**/HookInit.hpp", "**/Maps.*" }

	vpaths { ["Headers/*"] = "source/**.h",
			["Headers/Games/*"] = { "source/Games/**.h" },
			["Sources/*"] = { "source/**.c", "source/**.cpp" },
			["Sources/Games*"] = { "source/Games/**.c", "source/Games/**.cpp" },
			["Resources"] = "source/**.rc"
	}

	includedirs { "source/", "source/minhook/include" }

	files { "source/*.h", "source/*.cpp", "source/Games/*.h", "source/Games/*.cpp", "source/resources/*.rc" }

	-- Disable exceptions in WIL
	defines { "WIL_SUPPRESS_EXCEPTIONS" }

	-- Required when compiling for denuvo protected games
	defines { "DENUVO_SKIP_IAT" }

	-- Automated defines for resources
	defines { "rsc_Extension=\"%{prj.targetextension}\"",
			"rsc_Name=\"%{prj.name}\"" }


workspace "*"
	configurations { "Debug", "Release", "Master" }
	location "build"

	cppdialect "C++17"
	staticruntime "on"
	buildoptions { "/sdl" }
	warnings "Extra"

filter "configurations:Debug"
	defines { "DEBUG" }
	runtime "Debug"

 filter "configurations:Master"
	defines { "NDEBUG" }
	symbols "Off"

filter "configurations:not Debug"
	optimize "Speed"
	functionlevellinking "on"
	flags { "LinkTimeOptimization" }

filter { "platforms:Win32" }
	system "Windows"
	architecture "x86"

filter { "platforms:Win64" }
	system "Windows"
	architecture "x86_64"

filter { "toolset:*_xp"}
	defines { "WINVER=0x0501", "_WIN32_WINNT=0x0501" } -- Target WinXP
	buildoptions { "/Zc:threadSafeInit-" }

filter { "toolset:not *_xp"}
	defines { "WINVER=0x0601", "_WIN32_WINNT=0x0601" } -- Target Win7
	buildoptions { "/permissive-" }