#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include "bin_export.h"

bin_exporter::bin_exporter() : socket_(0), is_open_(false) {
    struct sockaddr_in server;
    
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(BINEX_SOCKET_PORT);

    if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error requesting socket from system\n");
        return;
    }
    fcntl(socket_, F_SETFL, O_NONBLOCK);
    
    if(bind(socket_, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0) {
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
    
    int client;
    socklen_t socksize = sizeof(struct sockaddr_in);
    struct sockaddr_in dest;
    
    
    if((client = accept(socket_, (struct sockaddr *)&dest, &socksize)) > 0) {
        clients_.insert(client);
    }
    
    for(auto fd : clients_) {
        if(write(fd, &data, 1) != 1) {
            // Remove the client so we don't keep sending them data
            clients_.erase(fd);
        }
    }
}
