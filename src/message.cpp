#include "message.h"

Message::Message(zmq::socket_t& socket) {
    while (true) {
        zmq::message_t message;
        socket.recv(&message);

        std::string as_string((char*) message.data(), message.size());
        frames_.push_back(as_string);

        // See if there are more
        int more = 0;
        size_t size = sizeof(more);
        socket.getsockopt(ZMQ_RCVMORE, &more, &size);
        if (!more) {
            return;
        }
    }
}

void Message::send(zmq::socket_t& socket) const {
    size_t count = frames_.size();
    for (auto const &part : frames_) {
        zmq::message_t message(part.size());
        memcpy(message.data(), part.data(), part.size());

        // SNDMORE on all but the last frame (count will hit 0)
        socket.send(message, (--count ? ZMQ_SNDMORE : 0));
    }
}

const std::vector<std::string>& Message::frames() const {
    return frames_;
}
