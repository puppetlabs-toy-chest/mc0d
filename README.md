This is a message broker implementing 0.2 of the mcollective zeromq protocol.

It's in C++ for peformance reasons, if you want just to understand the protocol
you may find the ruby implemention in
mcollective-zeromq-connector/bin/middleware easier to read.

# Configuration

## Logging configuration

Logging is handled by log4cxx, a port of the common log4j logging library.

```properties
# Add a rotating logfile at debug level
log4j.rootLogger = DEBUG, logfile

log4j.appender.logfile = org.apache.log4j.RollingFileAppender
log4j.appender.logfile.File = broker.log
log4j.appender.logfile.MaxFileSize = 100MB
log4j.appender.logfile.MaxBackupIndex = 5

log4j.appender.logfile.layout=org.apache.log4j.PatternLayout
log4j.appender.logfile.layout.ConversionPattern=%d [%t] %-5p %c - %m%n
```
