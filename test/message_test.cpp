#include "test.h"
#include "../src/message.h"

using ::testing::_;

TEST(MessageTest, BareConstructor) {
    Message msg;
    EXPECT_EQ(size_t(0), msg.frames().size());
}

TEST(MessageTest, VectorConstructor) {
    Message msg({ "pies" });
    EXPECT_EQ(size_t(1), msg.frames().size());
    EXPECT_EQ("pies", msg.frames().at(0));
}

// This can't easily be mocked, so just set up all of zmq
class MessageTest0MQ : public ::testing::Test {
  protected:
    virtual void SetUp() {
        context_ = new zmq::context_t;
        send_ = new zmq::socket_t(*context_, ZMQ_DEALER);
        recv_ = new zmq::socket_t(*context_, ZMQ_ROUTER);
        recv_->bind("inproc://test");
        send_->connect("inproc://test");
    }

    virtual void TearDown() {
        delete recv_;
        delete send_;
        delete context_;
    }
    zmq::context_t* context_;
    zmq::socket_t* send_;
    zmq::socket_t* recv_;
};

TEST_F(MessageTest0MQ, SocketConstructor) {
    zmq::message_t pies(4);
    memcpy(pies.data(), "pies", 4);
    send_->send(pies);

    // Will get you a 2-frame message, first being source
    Message msg(*recv_);
    EXPECT_EQ(size_t(2), msg.frames().size());
    EXPECT_EQ("pies", msg.frames().at(1));
}


TEST_F(MessageTest0MQ, Send) {
    Message msg({"pies!"});
    msg.send(*send_);

    // Will get you a 2-frame message, first being source
    zmq::message_t message;
    recv_->recv(&message);
    ASSERT_EQ(true, message.more());
    zmq::message_t next;
    recv_->recv(&next);

    ASSERT_EQ(size_t(5), next.size());
    std::string as_string((char*) next.data(), next.size());

    EXPECT_EQ("pies!", as_string);
}

TEST_F(MessageTest0MQ, SendRecv) {
    Message({"shoes!"}).send(*send_);
    Message shoes(*recv_);

    EXPECT_EQ(size_t(2), shoes.frames().size());
    EXPECT_EQ("shoes!", shoes.frames().at(1));
}

TEST_F(MessageTest0MQ, SendRecvMultipart) {
    Message({"shoes!", "socks"}).send(*send_);
    Message shoes(*recv_);

    EXPECT_EQ(size_t(3), shoes.frames().size());
    EXPECT_EQ("shoes!", shoes.frames().at(1));
    EXPECT_EQ("socks", shoes.frames().at(2));
}
