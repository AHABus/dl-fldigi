#ifndef BIN_EXPORT_H
#define BIN_EXPORT_H

#include <stdio.h>
#include <stdint.h>
#include <set>
#include "ring_buffer.h"

#define BINEX_SOCKET_PORT   (5555)

class bin_exporter {
    int                         socket_;
    bool                        is_open_;
    std::set<int>               clients_;
    
public:
    
    bin_exporter();
    ~bin_exporter();
    
    bool is_open() const { return is_open_; }
    
    void log(uint8_t data);
};


#endif
