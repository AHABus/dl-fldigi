#ifndef BIN_EXPORT_H
#define BIN_EXPORT_H

#include <stdio.h>
#include <stdint.h>
#include "ring_buffer.h"

#define BINEX_SOCKET_ADDRESS "/tmp/ahabus.sock-3"

class bin_exporter {
    FILE*                       socket_;
    int                         socketD_;
    bool                        is_open_;
    
public:
    
    bin_exporter();
    ~bin_exporter();
    
    bool is_open() const { return is_open_; }
    
    void log(uint8_t data);
};


#endif
