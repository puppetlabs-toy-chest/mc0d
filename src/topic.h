#ifndef TOPIC_H_
#define TOPIC_H_

#include <unordered_set>
#include <memory>
#include "client.h"

namespace mc0 {

class Topic {
  public:
    std::unordered_set<std::shared_ptr<Client>> subscribers;
};

}  // namespace mc0

#endif
