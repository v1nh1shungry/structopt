#ifndef STRUCTOPT_HPP
#define STRUCTOPT_HPP

#include <cmdline.h>
#include <magic_enum.hpp>
#include <string>
#include <tuple>
#include <type_traits>

namespace structopt {

namespace detail {

// from cppreference example https://en.cppreference.com/w/cpp/concepts/same_as
template <typename T, typename... U>
concept is_any_of = (std::same_as<T, U> || ...);

// count the number of fields in an aggregate
// from https://towardsdev.com/counting-the-number-of-fields-in-an-aggregate-in-c-20-c81aecfd725c

template <typename T>
concept aggregate = std::is_aggregate_v<T>;

// can convert to any types
struct any {
  template <typename T>
  constexpr operator T() const noexcept;
};

// https://zhuanlan.zhihu.com/p/624028822
template <class T>
consteval std::size_t aggregate_field_count(auto &&...args) {
  if constexpr (!requires{T{args...};})
    return sizeof...(args) - 1;
  else
    return aggregate_field_count<T>(std::forward<decltype(args)>(args)..., any{});
}

// now the dirty and evil part
// inspired by boost::pfr (https://github.com/boostorg/pfr)

template <std::size_t N>
struct size_tag {};

template <typename T>
auto tie_as_tuple(T &t) {
  return tie_as_tuple(t, size_tag<aggregate_field_count<T>()>{});
}

template <typename T>
auto tie_as_tuple(T &, size_tag<0>) {
  return std::tie();
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<1>) {
  auto &[a] = t;
  return std::tie(a);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<2>) {
  auto &[a, b] = t;
  return std::tie(a, b);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<3>) {
  auto &[a, b, c] = t;
  return std::tie(a, b, c);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<4>) {
  auto &[a, b, c, d] = t;
  return std::tie(a, b, c, d);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<5>) {
  auto &[a, b, c, d, e] = t;
  return std::tie(a, b, c, d, e);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<6>) {
  auto &[a, b, c, d, e, f] = t;
  return std::tie(a, b, c, d, e, f);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<7>) {
  auto &[a, b, c, d, e, f, g] = t;
  return std::tie(a, b, c, d, e, f, g);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<8>) {
  auto &[a, b, c, d, e, f, g, h] = t;
  return std::tie(a, b, c, d, e, f, g, h);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<9>) {
  auto &[a, b, c, d, e, f, g, h, i] = t;
  return std::tie(a, b, c, d, e, f, g, h, i);
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<10>) {
  auto &[a, b, c, d, e, f, g, h, i, j] = t;
  return std::tie(a, b, c, d, e, f, g, h, i, j);
}

// iterate a tuple
// from https://www.cppstories.com/2022/tuple-iteration-apply/
template <typename T, typename F>
void for_each_tuple(T &&t, F &&f) {
  std::apply
  (
    [&f]<typename ... Args>(Args && ... args) {
      (f(std::forward<Args>(args)), ...);
    }, std::forward<T>(t)
  );
}

} // namespace detail

namespace detail {

// supported structopt's option types
template <typename T>
concept opt_type = is_any_of<T, bool, int, std::string> || std::is_enum_v<T>;

template <opt_type T>
struct Info {
  char short_alias = ' ';
  std::string long_alias;
  std::string description;
  bool mandatory = false;
  T default_value{};
};

template <>
struct Info<bool> {
  char short_alias = ' ';
  std::string long_alias;
  std::string description;
};

template <opt_type T>
struct Opt {
  Info<T> info;
  T value;

  operator T() const { return value; }
};

struct OptBuilder {
  char short_alias;
  std::string long_alias;

  template <opt_type T>
  Opt<T> operator<=(Info<T> info) const {
    if (info.short_alias == ' ') {
      info.short_alias = short_alias;
    }
    if (info.long_alias.empty()) {
      info.long_alias = long_alias;
    }
    return Opt{ std::move(info), {} };
  }
};

struct Parser {
  int argc;
  char **argv;

  template <typename T>
  void to(T &option) const {
    cmdline::parser parser;

    auto t = tie_as_tuple(option);

    auto add = [&parser]<typename Arg>(const Opt<Arg> &arg) {
      if constexpr (std::same_as<Arg, bool>) {
        parser.add(arg.info.long_alias, arg.info.short_alias, arg.info.description);
      }
      else if constexpr (std::is_enum_v<Arg>) {
        cmdline::oneof_reader<std::string> range;
        auto names = magic_enum::enum_names<Arg>();
        for (auto &&name: names) {
          range.add(std::string{name});
        }
        parser.add<std::string>(arg.info.long_alias, arg.info.short_alias, arg.info.description,
                                arg.info.mandatory, std::string{magic_enum::enum_name(arg.info.default_value)},
                                range);
      }
      else {
        parser.add<Arg>(arg.info.long_alias, arg.info.short_alias, arg.info.description,
                        arg.info.mandatory, arg.info.default_value);
      }
    };
    for_each_tuple(t, add);

    parser.parse_check(argc, argv);

    auto get = [&parser]<typename Arg>(Opt<Arg> &arg) {
      if constexpr (std::same_as<Arg, bool>) {
        arg.value = parser.exist(arg.info.long_alias);
      } else if constexpr (std::is_enum_v<Arg>) {
        if (parser.exist(arg.info.long_alias)) {
          arg.value = magic_enum::enum_cast<Arg>(parser.get<std::string>(arg.info.long_alias)).value();
        } else {
          arg.value = arg.info.default_value;
        }
      }
      else {
        if (parser.exist(arg.info.long_alias)) {
          arg.value = parser.get<Arg>(arg.info.long_alias);
        } else {
          arg.value = arg.info.default_value;
        }
      }
    };
    for_each_tuple(t, get);
  }

  template <typename T>
  T to() const {
    T option;
    to(option);
    return option;
  }
};

} // namespace detail

inline detail::Parser from(int argc, char **argv) {
  return detail::Parser{.argc = argc, .argv = argv};
}

// import magic_enum's APIs
using namespace magic_enum;

} // namespace structopt

#define structopt_option(name, type) structopt::detail::Opt<type> name = structopt::detail::OptBuilder{ .short_alias = #name[0], .long_alias = #name } <= structopt::detail::Info<type>

#endif // !STRUCTOPT_HPP
