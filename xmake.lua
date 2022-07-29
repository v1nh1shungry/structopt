add_rules("mode.debug", "mode.release")

add_requires("cmdline", "magic_enum")

target("basic")
    set_languages("c++20")
    set_kind("binary")
    add_packages("cmdline", "magic_enum")
    add_includedirs("include")
    add_files("examples/basic.cpp")

target("enum")
    set_languages("c++20")
    set_kind("binary")
    add_packages("cmdline", "magic_enum")
    add_includedirs("include")
    add_files("examples/enum.cpp")
