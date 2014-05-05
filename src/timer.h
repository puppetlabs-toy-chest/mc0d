#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

/// A simple stopwatch/timer we can use for user feedback.  We use the
/// std::chrono::steady_clock as we don't want to be affected if the system
/// clock changed around us (think ntp skew/leapseconds).

class Timer {
  public:
    Timer() {
        reset();
    };

    double elapsedSeconds() {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(now - start_).count();
    };

    void reset() {
        start_ = std::chrono::steady_clock::now();
    };

  private:
    std::chrono::time_point<std::chrono::steady_clock> start_;
};

#endif  // TIMER_H_
