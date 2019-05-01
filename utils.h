#include <sys/time.h>
#include <stdlib.h>
#include <inttypes.h>

// PARSER CONSTANTS
#define IP_I 1
#define PORT_I 2
#define FILENAME_I 3
#define SIZE_I 4

// TRANSPORT PARAMETERS
#define BUFFER_SIZE 1000
#define SEGMENTS_LEN 500
#define MESSAGE_LEN 500

#define TIMEOUT_SEC 1
#define TIMEOUT_USEC 0

// SEGMENT STATUS CONSTANTS
#define SENT_REQUEST 1
#define RECEIVED_DATA 2
#define CAN_BE_REPLACED 3

#define min(X, Y) (((X) < (Y))? (X): (Y))

struct segment {
    // int id; //since our segments array will be cyclic, our id = size/BUFFER_SIZE 
    // might be problem if we get old udp and write it 
    int status;
    char buffer[BUFFER_SIZE];
};

void send_requests(int sockfd, size_t saved_prefix_len, struct segment *segments, size_t size, uint16_t port, char *ip_addr);
int wait_for_data(int sockfd);
int receive_data(int sockfd, struct segment *segments, size_t saved_prefix_len, char *addr, uint16_t port);
int write_prefix_and_get_len(size_t saved_prefix_len, struct segment *segments, FILE *file, size_t size);
