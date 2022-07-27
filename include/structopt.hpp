#ifndef STRUCTOPT_HPP
#define STRUCTOPT_HPP

#include <string>
#include <tuple>
#include <type_traits>
#include <cmdline.h>

namespace structopt {

namespace detail {

// from cppreference example https://en.cppreference.com/w/cpp/concepts/same_as
template <typename T, typename... U>
concept is_any_of = (std::same_as<T, U> || ...);

// count the number of fields in an aggregate
// from https://towardsdev.com/counting-the-number-of-fields-in-an-aggregate-in-c-20-c81aecfd725c

template <typename T>
concept aggregate = std::is_aggregate_v<T>;

template <typename T, typename ... Args>
concept aggregate_initializable = aggregate<T>
  && requires { T{std::declval<Args>()...}; };

// can convert to any types
struct any {
  template <typename T>
  constexpr operator T() const noexcept;
};

template <std::size_t I>
using indexed_any = any;

template <aggregate T, typename Indices>
struct aggregate_initializable_from_indices;

template <aggregate T, std::size_t ... Indices>
struct aggregate_initializable_from_indices<T, std::index_sequence<Indices...>>
  : std::bool_constant<aggregate_initializable<T, indexed_any<Indices>...>> {};

template <typename T, std::size_t N>
concept aggregate_initializable_with_n_args = aggregate<T>
  && aggregate_initializable_from_indices<T, std::make_index_sequence<N>>::value;

template <aggregate T, std::size_t N, bool C>
struct aggregate_field_count_impl
  : aggregate_field_count_impl<T, N + 1, aggregate_initializable_with_n_args<T, N + 1>> {};

template <aggregate T, std::size_t N>
struct aggregate_field_count_impl<T, N, false>
  : std::integral_constant<std::size_t, N - 1> {};

template <aggregate T>
struct aggregate_field_count : aggregate_field_count_impl<T, 0, true> {};

template <aggregate T>
constexpr inline std::size_t aggregate_field_count_v = aggregate_field_count<T>::value;

// now the dirty and evil part
// inspired by boost::pfr (https://github.com/boostorg/pfr)

template <std::size_t N>
struct size_tag {};

template <typename T>
auto tie_as_tuple(T &t) {
  return tie_as_tuple(t, size_tag<aggregate_field_count_v<T>>{});
}

template <typename T>
auto tie_as_tuple(T &t, size_tag<0>) {
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
concept opt_type = is_any_of<T,
    bool,
    int,
    std::string
  >;

template <opt_type T>
struct Info {
  char short_alias = ' ';
  std::string long_alias;
  std::string description;
  bool mandatory = false;
  T default_value{};
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

detail::Parser from(int argc, char **argv) {
  return detail::Parser{ .argc = argc, .argv = argv };
}

} // namespace structopt

#define structopt_option(name, type) structopt::detail::Opt<type> name = structopt::detail::OptBuilder{ .short_alias = #name[0], .long_alias = #name } <= structopt::detail::Info<type>

#endif // !STRUCTOPT_HPP
