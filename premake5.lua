workspace "Hazel"
	architecture "x64"
	startproject "Sandbox"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"

-- This next line copies and pastes the contents of the GLFW premake5 file into this spot (much like a C++ #include)
include "Hazel/vendor/GLFW"


project "Hazel"
	location "Hazel"
	kind "SharedLib"
	language "C++"
	buildoptions { "/utf-8"}

	postbuildmessage "COMPLETED %{prj.name} build!"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hzpch.h"
	pchsource "Hazel/src/hzpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}
	

	filter "system:windows"
		cppdialect "C++17"
		--staticruntime "On" -- this causes runtime problems (video 13 @22:58) read comments (messes up the buildoptions)
		systemversion "latest"


		defines 
		{
			"HZ_PLATFORM_WINDOWS",
			"HZ_BUILD_DLL";
		}

		postbuildcommands
		{		
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}


	filter "configurations:Debug"
		defines "HZ_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		buildoptions "/MD"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	buildoptions { "/utf-8"}


	postbuildmessage "COMPLETED %{prj.name} build!"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")



	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Hazel/vendor/spdlog/include",
		"Hazel/src"
	}

	links
	{
		"Hazel"
	}

	filter "system:windows"
		cppdialect "C++17"
		--staticruntime "On"  -- this causes runtime problems (video 13 @22:58) read comments (messes up the buildoptions)
		systemversion "latest"

		defines 
		{
			"HZ_PLATFORM_WINDOWS",			
		}
	

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		optimize "On"