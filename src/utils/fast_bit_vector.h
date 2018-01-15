//
// Created by shrum on 15.01.18.
//

#ifndef RBGGAMEMANAGER_FAST_RESET_VECTOR_H
#define RBGGAMEMANAGER_FAST_RESET_VECTOR_H

#include <vector>
class fast_bit_vector {
    std::vector<unsigned int> data;
    unsigned int threshold;
public:
    fast_bit_vector(std::size_t size, bool value)
            : data(size, (unsigned int) value)
    {
        threshold = 1;
    }

    bool operator[](std::size_t index) const
    {
        return data[index] >= threshold;
    }

    void set(std::size_t index)
    {
        data[index] = threshold;
    }

    void reset()
    {
        threshold++;
    }

    void resize(std::size_t new_size)
    {
        data.resize(new_size);
    }

    std::size_t size() const
    {
        return data.size();
    }

};


#endif //RBGGAMEMANAGER_FAST_RESET_VECTOR_H
