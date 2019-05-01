// Wiktor Garbarek 291963

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

// ./transport 127.0.0.1 40001 output 1100

void parse_args(int argc, char *argv[],
                char addr[][20], uint16_t *port, char filename[][256], size_t *size){
    
    if (argc != 5){
        fprintf(stderr, "Some arguments are missing - usage %s IP PORT FILENAME SIZE\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct in_addr checker;
    if (inet_aton(argv[IP_I], &checker) == 0) {
        fprintf(stderr, "Invalid address - expected form: x.y.z.t \n");
        exit(EXIT_FAILURE);
    }
    strcpy((char *) addr, argv[IP_I]);
    *port = atoi(argv[PORT_I]);

    if (sprintf((char *) filename, "%s", argv[FILENAME_I]) <= 0){
        fprintf(stderr, "Invalid filename\n");
        exit(EXIT_FAILURE);
    }

    *size = atol(argv[SIZE_I]);
}

int init_socket(){
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        fprintf(stderr, "socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in our_address;
	bzero (&our_address, sizeof(our_address));
	our_address.sin_family      = AF_INET;
	our_address.sin_port        = 0; //htons(12345); 
    // setting port 0 allows OS to pick unused port - we can retrieve it by getsockname(), 
    // for sake of assignment (clarity of code and testing via nc) port will be hardcoded
	our_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind (sockfd, (struct sockaddr*)&our_address, sizeof(our_address)) < 0) {
		fprintf(stderr, "bind error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

    return sockfd;
}


int main(int argc, char *argv[]){
    char addr[20];
    bzero(addr, 20);
    uint16_t port;
    char filename[256];
    size_t size;

    parse_args(argc, argv, &addr, &port, &filename, &size);

    int sockfd = init_socket();

    FILE *file = fopen(filename, "w");

    struct segment segments[SEGMENTS_LEN];
    bzero(segments, sizeof(segments));
    size_t saved_prefix_len = 0, prev_saved_len = 0;
    while(saved_prefix_len != size/BUFFER_SIZE + 1){
        send_requests(sockfd, saved_prefix_len, segments, size, port, addr);
        while (wait_for_data(sockfd) == 0){ 
            // wait_for_data uses select to avoid busy waiting
            //
            // this loop will almost never execute, 
            // but there is non-zero probability that we dont get any answer after sending first packs of requests
            send_requests(sockfd, saved_prefix_len, segments, size, port, addr);
        }
        //usleep(50000); 
        // usleep call is heuristic approach that allows us to receive more packets since wait_for_data loop stops when we receive first one  
        while(receive_data(sockfd, segments, saved_prefix_len, addr, port));
        prev_saved_len = saved_prefix_len;
        saved_prefix_len = write_prefix_and_get_len(saved_prefix_len, segments, file, size);

        if (prev_saved_len != saved_prefix_len)
            printf("Progress: %f %% \n", 100.0*saved_prefix_len/(1.0*(size/BUFFER_SIZE + 1)));
    }
    printf("Completed\n");

    close(sockfd);
    return 0;
}