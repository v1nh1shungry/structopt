add_rules("mode.debug", "mode.release")

add_requires("cmdline")

target("basic")
    set_languages("c++20")
    set_kind("binary")
    add_packages("cmdline")
    add_includedirs("include")
    add_files("examples/basic.cpp")
