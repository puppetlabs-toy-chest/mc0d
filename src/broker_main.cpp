#include "broker.h"
#include "logger.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "display help")
        ("bind", po::value<std::string>()->default_value("tcp://*:61616"), "address to bind")
        ("curve-private-key", po::value<std::string>()->required(), "private key")
        ("logger-config", po::value<std::string>()->default_value(""), "log4cxx properties file")
    ;

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc;
        return 0;
    }

    Logger logger(vm["logger-config"].as<std::string>());
    Broker broker(vm["bind"].as<std::string>(), vm["curve-private-key"].as<std::string>());
    broker.main_loop();
    return 0;
}
