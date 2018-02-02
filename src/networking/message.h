//
// Created by shrum on 01/02/18.
//

#ifndef RBGGAMEMANAGER_MESSAGE_H
#define RBGGAMEMANAGER_MESSAGE_H


#include <string>
#include <cstring>
#include "networking_utils.h"
#include "../game_components/search_context.h"

constexpr std::size_t header_length = 16;
constexpr std::size_t max_body_length = 65536;

class message {
    char data[header_length + max_body_length];
    std::size_t body_length;

    void encode_header()
    {
        char header[header_length + 1] = "";
        std::sprintf(header, "%8lu", body_length);
        std::memcpy(data, header, header_length);
    }

    message() = default;
public:
    message(const std::string& str)
    {
        body_length = std::min(str.length(), max_body_length);
        std::memcpy(body_ptr(), str.data(), get_body_length());
        encode_header();
    }

    static message empty()
    {
        return {};
    }

    static message client_move_message(const move& move)
    {
        std::string move_string = move_data_string(move);
        return {move_string};
    }

    bool decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, data, header_length);
        body_length = std::atoi(header);
        if (body_length > max_body_length)
        {
            body_length = 0;
            return false;
        }
        return true;
    }

    char* data_ptr()
    {
        return data;
    }

    char* body_ptr()
    {
        return data + header_length;
    }

    const char* body_ptr() const
    {
        return data + header_length;
    }

    std::size_t get_body_length() const
    {
        return body_length;
    }

    std::size_t length() const
    {
        return header_length + body_length;
    }
};



#endif //RBGGAMEMANAGER_MESSAGE_H
