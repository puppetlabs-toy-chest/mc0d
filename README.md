# mc0d

This is a message broker Marionette Collective 0MQ (mc0) protocol.

It's in C++ for peformance reasons.  If you want to understand the
[protocol][protocol] you may find the [ruby implementation][mc0d.rb] in the
[mcollective-zeromq-connector][connector] repository easier to read.

[protocol]: https://github.com/puppetlabs/mcollective-zeromq-connector/blob/master/PROTOCOL.md
[mc0d.rb]: https://github.com/puppetlabs/mcollective-zeromq-connector/blob/master/scripts/mc0d.rb
[connector]: https://github.com/puppetlabs/mcollective-zeromq-connector/

# Build dependencies

* cmake
* a C++11 compiler
* Boost >= 1.48
* log4cxx
* zeromq >= 4.0

# Usage

    ./bin/mc0d --curve-private-key broker.private

## Logging configuration

Logging is handled by log4cxx, a port of the common log4j logging library.

Set a path to a properties file using the `--logger-config` argument.

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

## Daemonising

We don't, it's expected you will be able to write an init script or other
process supervision configuration that can supervise mc0d for your
distribution.

## Scaling

We haven't yet been able to do extensive scale testing, but this can service a
few hundred nodes quite easily on a very modest linux vm.

# Limitations and other notes

zeromq's implementation of Curve does not appear to easily allow for trust
relationship between keys, so there's no existing model for 'create a ca to
issue keys and track revocations'.

zeromq additionally does not seem to allow for the validation of client keys,
by the server http://hintjens.com/blog:36, the clients can only verify the
server is the one indicated by `plugin.zeromq.broker.public_key`.

The implementation of the broker does not allow for persistent queues, and so
advanced uses of MCollective's asyncronous reply handling pattern such as
those explained [here][async_handling] are not currently supported.

[async_handling]: http://www.devco.net/archives/2012/08/19/mcollective-async-result-handling.php
