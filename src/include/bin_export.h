#ifndef BIN_EXPORT_H
#define BIN_EXPORT_H

#include <stdint.h>
#include "ring_buffer.h"

class bin_exporter {
    
public:
    
    bin_exporter();
    ~bin_exporter();
    
    void log(uint8_t data);
    
};


#endif
