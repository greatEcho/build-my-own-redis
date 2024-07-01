#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

void do_something(int connfd);

int main(void)
{
    // get file descriptor (handler) of the socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("can't get socket sockfd");
        errno = 0;
        exit(1);
    }

    int val = 1;
    // set options to our socket
    int ret_val  = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (ret_val) {
        perror("failed while setting socket options: ");
        errno = 0;
    }

    // bind the socket to an address
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);
    ret_val = bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr));
    if (ret_val) { // ret_val -> rv
        perror("failed while binding socket");
        errno = 0;
        // TO DO: make proper exit
        exit(1);
    }

    // listen
    ret_val = listen(sockfd, SOMAXCONN);
    if (ret_val) {
        perror("failed when try to listen");
        errno = 0;
        // TO DO: make proper exit
        exit(1);
    }

    // accept connections
    while (1) {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd < 0) {
            errno = 0;
            continue; // error
        }

        do_something(connfd);
        close(connfd);
    }


    return 0;
}

// interaction of server with connected client
void do_something(int connfd)
{
    char rbuf[64] = {};
    ssize_t ret_val = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (ret_val < 0) {
        perror("read failed");
        errno = 0;
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    ret_val = write(connfd, wbuf, strlen(wbuf));
    if (ret_val < 0) {
        perror("read failed");
        errno = 0;
        return;
    }
}
