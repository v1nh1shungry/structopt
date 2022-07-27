#include <structopt.hpp>
#include <iostream>

// This example is adapted from the official example in https://github.com/tanakh/cmdline
struct Option {
  structopt_option(host, std::string)
  {
    .description = "host name",
    .mandatory = true
  };
  structopt_option(port, int)
  {
    .description = "port number",
    .default_value = 80
  };
  structopt_option(type, std::string)
  {
    .description = "protocol type",
    .default_value = "http"
  };
  structopt_option(gzip, bool)
  {
    .short_alias = '\0',
    .description = "gzip when transfer",
  };
};

int main(int argc, char *argv[]) {
  auto opt = structopt::from(argc, argv).to<Option>();
  std::cout << opt.type.value << "://"
    << opt.host.value << ":"
    << opt.port.value << std::endl;
  if (opt.gzip.value) std::cout << "gzip" << std::endl;
  return 0;
}
