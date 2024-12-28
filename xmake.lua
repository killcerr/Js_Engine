add_rules("mode.debug", "mode.release")

add_repositories("levilamina https://github.com/LiteLDev/xmake-repo.git")

package("endstone")
    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/EndstoneMC/endstone")
    set_description("Endstone - High-level Plugin API for Bedrock Dedicated Servers (BDS), in both Python and C++")
    set_license("Apache-2.0")

    add_urls("https://github.com/EndstoneMC/endstone/archive/refs/tags/v$(version).tar.gz","https://github.com/EndstoneMC/endstone.git")
    add_versions("0.5.6","2a991d4009025a10c4469f160a15ad40594f82ebd08dab510ea0f42d61b7c8dd")
    add_patches("0.5.6", "https://raw.githubusercontent.com/engsr6982/Js_Engine/refs/heads/develop/patch/cxx20.patch",
                        "547ae3d325b8deb68747179b6bc3aa8772ba4efe36263bf31f34be7a3aac2ceb")
    if (is_plat("linux")) then 
        add_patches("0.5.6", "https://raw.githubusercontent.com/engsr6982/Js_Engine/refs/heads/develop/patch/linux.patch",
                        "1e7c6a961abf821803b42dcd43d1c88524caa8012e333b9e47ec6faa00037c74")   
    end

    on_install("windows", "linux", function (package)
        os.cp("include", package:installdir())
    end)
package_end()


add_requires(
    "expected-lite 0.8.0",
    "entt 3.14.0",
    "microsoft-gsl 4.0.0",
    "nlohmann_json 3.11.3",
    "boost 1.85.0",
    "glm 1.0.1",
    "concurrentqueue 1.0.4",
    "endstone 0.5.6",
    "magic_enum 0.9.7"
)

local fmt_version = "fmt >=10.0.0 <11.0.0";
if is_plat("windows") then 
    add_requires(fmt_version)
elseif is_plat("linux") then
    set_toolchains("clang")
    add_requires("libelf 0.8.13")
    add_requires(fmt_version, {configs = {header_only = true}})
end

if is_plat("windows") then
    if not has_config("vs_runtime") then
        set_runtimes("MD")
    end
end


target("Js_Engine")
    add_defines(
        "NOMINMAX",
        "UNICODE",
        "_AMD64_"
    )
    add_files("src/**.cc")
    add_includedirs("src")
    add_packages(
        "fmt",
        "expected-lite",
        "entt",
        "microsoft-gsl",
        "nlohmann_json",
        "boost",
        "glm",
        "concurrentqueue",
        "endstone",
        "magic_enum"
    )
    set_kind("shared")
    set_languages("cxx20")
    set_symbols("debug")
    -- set_exceptions("none")

    -- EndStone Entt
    add_defines("ENTT_SPARSE_PAGE=2048")
    add_defines("ENTT_PACKED_PAGE=128")


    -- 根据不同平台设定编译参数
    if is_plat("windows") then
        add_cxflags(
            "/EHa",
            "/utf-8",
            -- "/W4",
            "/sdl"
        )
    elseif is_plat("linux") then
        add_cxflags(
            "-fPIC",
            "-stdlib=libc++",
            "-fdeclspec",
            {force = true}
        )
        add_ldflags(
            "-stdlib=libc++",
            {force = true}
        )
        add_packages("libelf")
        add_syslinks("dl", "pthread", "c++", "c++abi")
    end

    -- ScriptX
    add_includedirs("third-party/scriptx/src/include")
    add_files(
        "third-party/scriptx/src/**.cc",
        "third-party/scriptx/backend/V8/**.cc"
    )
    add_defines(
        "SCRIPTX_BACKEND_V8",
        "SCRIPTX_BACKEND_TRAIT_PREFIX=../third-party/scriptx/backend/V8/trait/Trait"
    )

    add_includedirs(
        "third-party/nodejs/win/include",
        "third-party/nodejs/win/include/v8",
        "third-party/nodejs/win/include/uv"
    )
    if is_plat("windows") then
        add_links("third-party/nodejs/win/lib/libnode.lib")
    elseif is_plat("linux") then
        add_rpathdirs("$ORIGIN/../") -- ./plugins/js_engine/libnode.so.93
        add_links("third-party/nodejs/linux/libnode.so")
    end


    if is_mode("debug") then
        add_defines("DEBUG")
    end

    set_basename("js_engine")
    after_build(function(target)
        local output_dir = path.join(os.projectdir(), "bin")
        local ext = ".dll";
        if (is_plat("linux")) then
            ext = ".so";
        end

        os.cp(target:targetfile(), path.join(output_dir, target:basename() .. ext))

        os.cp(path.join(os.projectdir(), "lib"), path.join(os.projectdir(), "bin", "js_engine"))

        local pdb_path = path.join(output_dir, target:name() .. ".pdb")
        if os.isfile(target:symbolfile()) then 
            os.cp(target:symbolfile(), pdb_path) 
        end

        cprint("${bright green}[plugin Packer]: ${reset}plugin already generated to " .. output_dir)
    end)