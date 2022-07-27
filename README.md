# structopt

A lovely structopt library for C++, inspired by [TeXitoi/structopt](https://github.com/TeXitoi/structopt) and powered by [tanakh/cmdline](https://github.com/tanakh/cmdline) and ❤️

# Quick Start

So, you want to parse command line arguments by just defining a `struct`? Here you go!

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

**Attention! If the option is `bool`, `mandatory` and `default_value` will be ignored.**

```bash
❯ ./basic
usage: ./basic --host=string [options] ...
options:
  -h, --host    host name (string)
  -p, --port    port number (int [=80])
  -t, --type    protocol type (string [=http])
  -?, --help    print this message
```

Wait wait! I forget to tell you how to parse the command line arguments and use these arguments!

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

## Installation

structopt is header-only, but it depends on [tanakh/cmdline](https://github.com/tanakh/cmdline). So make sure you have installed cmdline (header-only too) in your `include` path.

## Build Examples

To build and run the examples in this repo, you don't have to install cmdline manually. [xmake](https://xmake.io/) will handle this for you. xmake is an amazing build utility for C/C++ 👍.

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

2. The `struct` used to define the command line arguments must be [Aggregate initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization). For short, **no user-declared constructors and no inheritance. Always use `structopt_option(name, type)` to add new option.**

3. Because of the implementation, **there're at most 10 arguments**.

4. For now structopt's parsing work is based on cmdline. And structopt doesn't add any new functions about parsing. So it can be seen as a wrapper of cmdline.

# Roadmap

- [ ] `enum` support
- [ ] more arguments support

# Contributing

Contributions are welcome! ❤️

# Acknowledgement

structopt depends on or is inspired by the following projects. Great thanks! 👍

* [xmake](https://xmake.io/)
* [tanakh/cmdline](https://github.com/tanakh/cmdline)
* [TeXitoi/structopt](https://github.com/TeXitoi/structopt)