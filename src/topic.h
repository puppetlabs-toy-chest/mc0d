#ifndef TOPIC_H_
#define TOPIC_H_

#include <unordered_set>
#include "client.h"

class Topic {
  public:
    std::unordered_set<Client*> subscribers;
};

#endif
