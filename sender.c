// Wiktor Garbarek 291963

#include <netinet/ip.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "utils.h"

void send_request(int sockfd, size_t offset, size_t size, uint16_t port, char *ip_addr){
    char message[MESSAGE_LEN];
    sprintf(message, "GET %ld %ld\n", offset, size);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip_addr, &server_address.sin_addr);

    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        fprintf(stderr, "sendto");
        exit(EXIT_FAILURE);
    }

}


void send_requests(int sockfd, size_t saved_prefix_len, struct segment *segments, size_t size, uint16_t port, char *ip_addr){
    for(size_t i=saved_prefix_len; i < min(saved_prefix_len + SEGMENTS_LEN, size/BUFFER_SIZE); i++){
        if (segments[i % SEGMENTS_LEN ].status != RECEIVED_DATA) {
            send_request(sockfd, i*BUFFER_SIZE, BUFFER_SIZE, port, ip_addr);
            segments[i % SEGMENTS_LEN].status = SENT_REQUEST;
        }
    }
    if (segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].status != RECEIVED_DATA){
        send_request(sockfd, (size/BUFFER_SIZE) * BUFFER_SIZE, size % BUFFER_SIZE, port, ip_addr);
        segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].status = SENT_REQUEST;
    }
}