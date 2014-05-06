#ifndef LOGGER_H_
#define LOGGER_H_

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

// Convenience macros so we can replace loggers in the futures
// DECLARE_LOGGER_NAMESPACE is intended to be called once a compile unit like
// so:
//     DECLARE_LOGGER_NAMESPACE("main")
#define DECLARE_LOGGER_NAMESPACE(name) \
    static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger((name)));

#define LOGGER_TRACE(log) LOG4CXX_TRACE(logger, log);
#define LOGGER_DEBUG(log) LOG4CXX_DEBUG(logger, log);
#define LOGGER_INFO(log)  LOG4CXX_INFO(logger, log);
#define LOGGER_WARN(log)  LOG4CXX_WARN(logger, log);
#define LOGGER_ERROR(log) LOG4CXX_ERROR(logger, log);

namespace mc0 {

class Logger {
  public:
    Logger() = delete;
    explicit Logger(std::string path) {
        if (path.size()) {
            log4cxx::PropertyConfigurator::configure(path);
        }
        else {
            log4cxx::BasicConfigurator::configure();
        }
    }
};

}  // namespace mc0

#endif  // LOGGER_H_
