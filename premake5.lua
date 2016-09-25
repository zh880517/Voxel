
local BuilPath = "Build/" .. _ACTION  --工程路径
local ObjPath = "Build/ObjOut/" .. _ACTION  -- 编译中间文件输出目录
local OutPath = "%{cfg.buildcfg}_%{cfg.platform}"

workspace "D3D"
    configurations { "Debug", "Release" }--项目配置类型
    platforms { "X32", "X64"}
    location(BuilPath)--解决方案目录

    flags {"C++11", "MultiProcessorCompile"}
    startproject "D3DTest" 

	filter "configurations:Debug" --编译配置
		defines { "DEBUG" } --预编译宏
		flags { "Symbols" } -- 生成pdb文件

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On" --开启优化选项
	-- filter { "platforms:*32" }
    -- 	architecture "x86"
	-- filter { "platforms:*64" }
    -- 	architecture "x86_64"

    
project "D3DTest"
   kind "WindowedApp" 
   language "C++" 
   flags{"WinMain"}
   targetdir("bin/"..OutPath) --生成文件目录
   location(BuilPath.."/D3DTest") --工程目录
   objdir(ObjPath.."/D3DTest/%{cfg.buildcfg}")
   dependson {"DXUTOpt", "LibVoxel"} --编译依赖项
   includedirs { "DXUT", "LibVoxel"}
   links {"LibVoxel", "DXUT", "DXUTOpt", "d3dcompiler", "dxguid", "winmm", "comctl32", "usp10", "imm32", "version"}
   files { "D3DTest/**.h", "D3DTest/**.cpp" }
    
project "DXUT"
   kind "StaticLib" 
   language "C++" 
   targetdir("lib/"..OutPath) --生成文件目录
   location(BuilPath.."/DXUT") --工程目录
   objdir(ObjPath.."/DXUT/%{cfg.buildcfg}")
   includedirs {"DXUT/Core"}
   
   files { "DXUT/Core/**.h", "DXUT/Core/**.cpp" }

project "DXUTOpt"
   kind "StaticLib" 
   language "C++" 
   targetdir("lib/"..OutPath) --生成文件目录
   location(BuilPath.."/DXUTOpt") --工程目录
   objdir(ObjPath.."/DXUTOpt/%{cfg.buildcfg}")
   includedirs {"DXUT/Core", "DXUT/Optional"}
   dependson {"DXUT"}
   
   files { "DXUT/Optional/**.h", "DXUT/Optional/**.cpp" }

project "LibVoxel"
   kind "StaticLib" 
   language "C++" 
   targetdir("lib/"..OutPath) --生成文件目录
   location(BuilPath.."/LibVoxel") --工程目录
   objdir(ObjPath.."/LibVoxel/%{cfg.buildcfg}")
   includedirs {"LibVoxel"}
    files { "LibVoxel/**.h", "LibVoxel/**.cpp" }