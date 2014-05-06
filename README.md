# mc0d

This is a message broker implementing 0.3 of the mcollective zeromq (mc0) protocol.

It's in C++ for peformance reasons.  If you want to understand the protocol
you may find the ruby implemention in mcollective-zeromq-connector/scripts/mc0d.rb
easier to read.

# Build dependencies

cmake
a C++11 compiler
Boost
log4cxx
zeromq 4.0

# Configuration

## Logging configuration

Logging is handled by log4cxx, a port of the common log4j logging library.

```properties
# Add a rotating logfile at debug level
log4j.rootLogger = DEBUG, logfile

log4j.appender.logfile = org.apache.log4j.RollingFileAppender
log4j.appender.logfile.File = mc0d.log
log4j.appender.logfile.MaxFileSize = 100MB
log4j.appender.logfile.MaxBackupIndex = 5

log4j.appender.logfile.layout=org.apache.log4j.PatternLayout
log4j.appender.logfile.layout.ConversionPattern=%d [%t] %-5p %c - %m%n
```
