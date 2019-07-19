//
// Created by shrum on 02.07.19.
//

#ifndef RBGGAMEMANAGER_MESSAGE_H
#define RBGGAMEMANAGER_MESSAGE_H

#include <string>
#include <cstring>

namespace rbg {
  constexpr std::size_t kHeaderLength = 5;
  constexpr std::size_t kMaxBodyLength = 99999;

  class Message {
  public:
    explicit Message(const std::string &str) : body_length_(0) {
      body_length_ = std::min(str.length() + 1, kMaxBodyLength);
      std::memcpy(body_ptr(), str.c_str(), body_length());
      EncodeHeader();
    }

    Message() = default;

    bool DecodeHeader() {
      char header[kHeaderLength + 1] = "";
      std::strncat(header, data_, kHeaderLength);
      body_length_ = static_cast<size_t>(std::atoi(header));
      if (body_length_ > kMaxBodyLength) {
        body_length_ = 0;
        return false;
      }
      return true;
    }

    char *data_ptr() {
      return data_;
    }

    char *body_ptr() {
      return data_ + kHeaderLength;
    }

    const char *body_ptr() const {
      return data_ + kHeaderLength;
    }

    std::size_t body_length() const {
      return body_length_;
    }

    std::size_t length() const {
      return kHeaderLength + body_length_;
    }

  public:
    void EncodeHeader() {
      char header[kHeaderLength + 1] = "";
      std::sprintf(header, ("%" + std::to_string(kHeaderLength) + "lu").c_str(), body_length_);
      std::memcpy(data_, header, kHeaderLength);
    }


    char data_[kHeaderLength + kMaxBodyLength]{};
    std::size_t body_length_;
  };
}

#endif //RBGGAMEMANAGER_MESSAGE_H
