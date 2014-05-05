#include "test.h"
#include "../src/message.h"

TEST(MessageTest, Constructor) {
    Message msg({});
    EXPECT_EQ(size_t(0), msg.frames().size());
}
