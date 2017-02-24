#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>

template <typename T, size_t C>
class ring_buffer {
public:
    
    typedef T           value_type;
    typedef T&          reference;
    typedef const T&    const_reference;
    
    /// Creates a new ring buffer.
    ring_buffer()
        : front_(0), back_(0) {}
    
    /// Returns the number of items available in the buffer.
    size_t size() const {
        return (C + front_ - back_) % C;
    }
    
    /// Pushes [value] to the front of the buffer.
    void push_front(const_reference value) {
        size_t next_head = (front_ + 1) % C;
        if(front_ == back_) { return; }
        data_[front_] = value;
        front_ = next_head;
    }
    
    /// Consumes the item at the back of the buffer.
    void pop_back() {
        if(front_ == back_) { return; }
        back_ = (back_ + 1) % C;
    }
    
    /// Returns a constant reference to the next item available in the buffer.
    const_reference back() const {
        return data_[back_];
    }
    
    /// Returns a the next item available in the buffer.
    value_type cback() const {
        return data_[back_];
    }
    
    /// Clears the buffer.
    void clear() {
        front_ = back_ = 0;
    }
    
private:
    size_t              front_;
    size_t              back_;
    value_type          data_[C];
};

#endif /* RING_BUFFER_H */
