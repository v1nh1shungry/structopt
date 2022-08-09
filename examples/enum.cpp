#include <structopt.hpp>
#include <iostream>

struct Opt {
  structopt_option(host, std::string)
  {
    .description = "host name",
    .mandatory = true
  };
  enum class ProtocolType { http, https, ssh, ftp };
  structopt_option(type, ProtocolType)
  {
    .description = "protocol type",
    .default_value = ProtocolType::http
  };
};

int main(int argc, char *argv[]) {
  Opt opt = structopt::from(argc, argv).to<Opt>();
  std::cout << structopt::enum_name(opt.type.value) << "://"
    << opt.host.value << std::endl;
  return 0;
}
