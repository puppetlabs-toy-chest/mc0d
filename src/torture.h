#ifndef TORTURE_H_
#define TORTURE_H_

#include <string>
#include <iostream>
#include <fstream>

namespace mc0 {

class Torture {
  public:
    Torture(std::string target, std::string public_key) :
        target_(target) {
        std::ifstream input(public_key);
        input >> public_key_;
    }
    void main_loop();
  private:
    void chattyClient(int id, int count);
    std::string target_;
    std::string public_key_;
};

}  // namespace mc0

#endif
