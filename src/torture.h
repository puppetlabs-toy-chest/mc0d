#ifndef TORTURE_H_
#define TORTURE_H_

#include <string>

class Torture {
  public:
    Torture(std::string target, std::string public_key) :
        target_(target), public_key_(public_key) {}
    void main_loop();
  private:
    void chattyClient(int id);
    std::string target_;
    std::string public_key_;
};

#endif
