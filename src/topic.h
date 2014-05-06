#ifndef TOPIC_H_
#define TOPIC_H_

#include <unordered_set>
#include "client.h"

namespace mc0 {

class Topic {
  public:
    std::unordered_set<Client*> subscribers;
};

}  // namespace mc0

#endif
