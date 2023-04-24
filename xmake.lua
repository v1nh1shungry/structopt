add_rules("mode.debug", "mode.release")
add_requires("cmdline", "magic_enum")
set_defaultmode('debug')

local config = function()
    add_includedirs("include")
    add_packages("cmdline", "magic_enum")
    set_languages("c++20")
    set_warnings('allextra')
end

target("basic")
    add_files("examples/basic.cpp")
    config()

target("enum")
    add_files("examples/enum.cpp")
    config()
