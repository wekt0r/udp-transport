// Wiktor Garbarek 291963

#include <stdio.h>
#include <inttypes.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include "utils.h"

void set_timeout(struct timeval *tv){
    tv->tv_sec = TIMEOUT_SEC; tv->tv_usec = TIMEOUT_USEC;
}

int wait_for_data(int sockfd){
    struct timeval timeout;
    set_timeout(&timeout);

    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);

    int ready = select(sockfd+1, &descriptors, NULL, NULL, &timeout); 
    if (ready < 0){
        fprintf(stderr, "select");
        exit(EXIT_FAILURE);
    }
    return ready;
}

size_t segment_id_from_offset(size_t offset){
    return (offset/BUFFER_SIZE) % SEGMENTS_LEN;
}


void save_data_to_segments(struct segment *segments, uint8_t *buffer, size_t offset, size_t size){
    memcpy(segments[segment_id_from_offset(offset)].buffer, strchr( (char *) buffer, '\n') + 1, size);
    segments[segment_id_from_offset(offset)].status = RECEIVED_DATA;
}

int receive_data(int sockfd, struct segment *segments, size_t saved_prefix_len, char *addr, uint16_t port){
    struct sockaddr_in  sender;	
    socklen_t           sender_len = sizeof(sender);
    u_int8_t            buffer[IP_MAXPACKET+1];

    ssize_t datagram_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
    
    if (datagram_len < 0 && errno == EWOULDBLOCK){
        return 0;
    }
    
    if (datagram_len < 0) {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
        return EXIT_FAILURE;
    }
    

    char sender_ip_str[20];
    size_t size, offset;
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
    if (strcmp(sender_ip_str, addr) == 0 && ntohs(sender.sin_port) == port){

        sscanf((char *) buffer, "DATA %ld %ld\n", &offset, &size);
        if (offset/BUFFER_SIZE >= saved_prefix_len && 
            offset/BUFFER_SIZE < saved_prefix_len + SEGMENTS_LEN &&
            segments[segment_id_from_offset(offset)].status != RECEIVED_DATA)
           save_data_to_segments(segments, buffer, offset, size);
    }
    return 1;
}