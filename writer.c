// Wiktor Garbarek 291963

#include <unistd.h>
#include <stdio.h>
#include "utils.h"

int write_prefix_and_get_len(size_t saved_prefix_len, struct segment *segments, int filefd, size_t size){
    size_t currently_saved_prefix = saved_prefix_len;
    for(size_t i=currently_saved_prefix; i < min(currently_saved_prefix + SEGMENTS_LEN, size/BUFFER_SIZE); i++){
        if (segments[i % SEGMENTS_LEN].status == RECEIVED_DATA){
            if (write(filefd, segments[i % SEGMENTS_LEN].buffer, BUFFER_SIZE) < 0){
                handle_error("write");
            }
            segments[i % SEGMENTS_LEN].status = CAN_BE_REPLACED;
            currently_saved_prefix++;
        } else {
            return currently_saved_prefix;
        }
    }
    if (segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].status == RECEIVED_DATA){
        if (write(filefd, segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].buffer, size % BUFFER_SIZE) < 0){
            handle_error("write");
        }
        segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].status = CAN_BE_REPLACED;
        currently_saved_prefix++;
    }
    return currently_saved_prefix;
}
