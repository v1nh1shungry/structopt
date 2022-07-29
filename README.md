# structopt

A lovely structopt library for C++! Parse command line arguments by defining a struct! ❤️

# Quick Start

**Make sure you get a fancy compiler supports C++20!**

So, you want to parse command line arguments by just defining a `struct`? Here we go!

First, let's define a `struct`,

```cpp
struct Option {
  // TODO
};
```

Suppose we want to add a `host` option

```cpp
struct Option {
  structopt_option(host, std::string)
  {
    .description = "host name",
    .mandatory = true
  };
}
```

That's it. And let me tell you what happened. `structopt_option(name, type)` is a `macro`. This example defines an option which is named `host` and has a `std::string` value. The `mandatory` attribute decides if the command line arguments must contain `host`. By default its value is `false`, so if you don't want this you can just ignore it.

Now let's run it without any command line arguments,

```bash
❯ ./basic
usage: ./basic --host=string [options] ...
options:
  -h, --host    host name (string)
  -?, --help    print this message
```

By default, the long option name is exactly the name you define it. And the short one is the first character of the name.

Move on. We also want a `port` number whose default value is `80`,

```cpp
// inside the struct Option
structopt_option(port, int)
{
  .description = "port number",
  .default_value = 80
};
```

Run!

```bash
usage: ./basic --host=string [options] ...
options:
  -h, --host    host name (string)
  -p, --port    port number (int [=80])
  -?, --help    print this message
```

Cool! But I want a flag and I don't want a short name,

```cpp
// inside the struct Option
structopt_option(gzip, bool)
{
  .short_alias = '\0'
  .description = "gzip when transfer",
}
```

By assigning `\0` to the `short_alias` attribute, we disable the short option name. **However, we can't ban the long one for now.**

And yes, the name `long_alias` reminds me. If you want a different long option name, you can `.long_alias = "<new name>"`. `short_alias` works too.

**Attention! If the option is `bool`, `mandatory` and `default_value` are not available.**

```bash
❯ ./basic
usage: ./basic --host=string [options] ...
options:
  -h, --host    host name (string)
  -p, --port    port number (int [=80])
  -t, --type    protocol type (string [=http])
  -?, --help    print this message
```

One more request! I want the input to be restricted within several values. Well, we can achieve it by using `enum`!

```cpp
// inside struct Option
// first define available values with an enum
enum ProtocolType { http, https, ssh, ftp };
structopt_option(type, ProtocolType)
{
  .description = "protocol type",
  .default_value = ProtocolType::http
};
```

That's it! Have a try!

```bash
❯ ./enum --host github.com --type xyz
option value is invalid: --type=xyz
usage: ./enum --host=string [options] ...
options:
  -h, --host    host name (string)
  -t, --type    protocol type (string [=http])
  -?, --help    print this message
```

Wait wait! I forgot to tell you how to parse the command line arguments and use these arguments!

```cpp
int main(int argc, char *argv[]) {
  auto opt = structopt::from(argc, argv).to<Option>();
  // Option opt;
  // structopt::from(argc, argv).to(opt);
  std::string host = opt.host;
  int port = opt.port;
  bool gzip = opt.gzip;
}
```

Hooray! It's sweet, isn't it? 🎉

Check the complete example in [basic.cpp](examples/basic.cpp). And yes, this example is adapted from the official example in [tanakh/cmdline](https://github.com/tanakh/cmdline).

# Usage

## Install Manually

structopt is header-only, so you can just download [structopt.hpp](include/structopt.hpp) and place it to your `include` path. Since structopt depends on [tanakh/cmdline](https://github.com/tanakh/cmdline),  make sure you have installed cmdline (header-only too) in your `include` path.

## Install Via xmake

[xmake](https://xmake.io/) is an amazing build utility for C/C++ 👍. If you use xmake in your project, you can paste the following configuration to your `xmake.lua`, then you can use `add_requires(structopt)` and `add_pacakges(structopt)` to add structopt to your project.

```lua
package("structopt")

    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/v1nh1shungry/structopt")
    set_description("A lovely structopt library for C++! Parse command line arguments by defining a struct! ❤️")

    add_urls("https://github.com/v1nh1shungry/structopt.git")
    add_versions("2022.7.29", "1ea5e6670b201c9210f7dddfbb80fbebad5ddbf6")

    add_deps("cmdline")
    add_deps("magic_enum")

    on_install("linux", "mingw", function (package)
      os.cp("include/structopt.hpp", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <structopt.hpp>
            static void test() {
                struct Opt {
                    structopt_option(host, std::string)
                    {
                        .description = "host name",
                        .mandatory = true
                    };
                };
                int argc = 1;
                char arg[] = "./test";
                char *argv = arg;
                Opt opt = structopt::from(argc, &argv).to<Opt>();
            }
        ]]}, {configs = {languages = "c++20"}}))
    end)

package_end()
```

# Build & Run Examples

1. You should first install xmake. Check more details about how to install it in your platform 👉 [Installation](https://xmake.io/#/guide/installation).

2. Clone this project and just `xmake`!

   ```bash
   git clone https://github.com/v1nh1shungry/structopt.git
   cd structopt
   xmake
   # then you've finished building
   # run the basic example without arguments
   xmake run basic
   # run it with arguments
   xmake run basic --host=github.com
   ```

# Limitations

I have to admit that structopt is more an experimental project than a serious one.

1. For now structopt only supports three types:
   * `bool`
   * `int`
   * `std::string`
   * `enum`

2. The `struct` used to define the command line arguments must be [Aggregate initializable](https://en.cppreference.com/w/cpp/language/aggregate_initialization). For short, **no user-declared constructors and no inheritance. Always use `structopt_option(name, type)` to add new option.**

3. Because of the implementation, **there're at most 10 arguments**.

4. For now structopt's parsing work is based on cmdline. And structopt doesn't add any new functions about parsing. So it is just a wrapper for cmdline.

5. Since cmdline doesn't support msvc (https://github.com/tanakh/cmdline/issues/6) and structopt depends on it, structopt doesn't support msvc too. MinGW is okay.

# Roadmap

- [x] `enum` support
- [ ] more arguments support
- [ ] remove dependency on cmdline

# Contributing

Contributions are welcome! ❤️

# Acknowledgement

structopt depends on the following projects. Great thanks! 👍

* [xmake](https://xmake.io/)
* [tanakh/cmdline](https://github.com/tanakh/cmdline)
* [Neargye/magic_enum](https://github.com/Neargye/magic_enum)

structopt is inspired by the following projects. 👍
* [TeXitoi/structopt](https://github.com/TeXitoi/structopt)
* [boostorg/pfr](https://github.com/boostorg/pfr)