#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include "bin_export.h"

bin_exporter::bin_exporter() : socket_(0), client_(-1), is_open_(false) {
    socket_ = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(socket_ < 0) {
        perror("error requesting socket from system\n");
        return;
    }
    
    fcntl(socket_, F_SETFL, O_NONBLOCK);
    struct sockaddr_un name;
    unlink(BINEX_SOCKET_ADDRESS);
    
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, BINEX_SOCKET_ADDRESS);
    
    if(bind(socket_, (struct sockaddr*)&name, SUN_LEN(&name)) < 0) {
        perror("bind");
        close(socket_);
        return;
    }
    
    if(listen(socket_, 5) < 0) {
        perror("listen");
        close(socket_);
        return;
    }
    
    is_open_ = true;
}

bin_exporter::~bin_exporter() {
    close(socket_);
}

void bin_exporter::log(uint8_t data) {
    if(!is_open_) { return; }
    
    if(client_ < 0) {
        client_ = accept(socket_, NULL, NULL);
        if(client_ < 0) { return; }
        fcntl(client_, F_SETFL, O_NONBLOCK);
    }
    
    write(client_, &data, 1);
}
