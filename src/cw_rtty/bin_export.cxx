#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "bin_export.h"

bin_exporter::bin_exporter() : socket_(NULL), socketD_(0), is_open_(false) {
    socketD_ = socket(PF_LOCAL, SOCK_STREAM, 0);
    if(socketD_ < 0) {
        perror("error requesting socket from system\n");
        return;
    }
    
    struct sockaddr_un name;
    
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, "/tmp/ahabus.sock-tmp");
    size_t size = (offsetof (struct sockaddr_un, sun_path)
          + strlen (name.sun_path));
    
    if(bind(socketD_, (struct sockaddr*)&name, size) < 0) {
        perror("bind");
        close(socketD_);
        return;
    }
    
    if(listen(socketD_, 5) < 0) {
        perror("listen");
        close(socketD_);
        return;
    }
    
    socket_ = fdopen(socketD_, "r");
    if(socket_ == NULL) {
        perror("file ptr");
        close(socketD_);
        return;
    }
    
    is_open_ = true;
}

bin_exporter::~bin_exporter() {
    fclose(socket_);
    close(socketD_);
}

void bin_exporter::log(uint8_t data) {
    if(!is_open_) { return; }
    fwrite(&data, 1, 1, socket_);
    //data_buffer_.push_front(data);
}
