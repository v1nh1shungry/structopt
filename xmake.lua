add_rules("mode.debug", "mode.release")

package("cmdline")

  set_kind("library", {headeronly = true})
  set_homepage("https://github.com/tanakh/cmdline")
  set_description("A Command Line Parser")
  set_license("BSD-3-Clause")

  add_urls("https://github.com/tanakh/cmdline.git")
  add_versions("2014.2.4", "e4cd007fb8f0314002d9a5b4d82939106e4144e4")

  on_install(function (package)
    os.cp("cmdline.h", package:installdir("include"))
  end)

  on_test(function (package)
    assert(package:has_cxxincludes("cmdline.h"))
  end)

package_end()

add_requires("cmdline")

target("basic")
    set_languages("c++20")
    set_kind("binary")
    add_packages("cmdline")
    add_includedirs("include")
    add_files("examples/basic.cpp")
