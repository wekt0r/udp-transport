// Wiktor Garbarek 291963

#include <stdio.h>
#include "utils.h"

int write_prefix_and_get_len(size_t saved_prefix_len, struct segment *segments, FILE *file, size_t size){
    size_t currently_saved_prefix = saved_prefix_len;
    for(size_t i=currently_saved_prefix; i < min(currently_saved_prefix + SEGMENTS_LEN, size/BUFFER_SIZE); i++){
        if (segments[i % SEGMENTS_LEN].status == RECEIVED_DATA){
            fwrite(segments[i % SEGMENTS_LEN].buffer, sizeof(char), BUFFER_SIZE, file);
            segments[i % SEGMENTS_LEN].status = CAN_BE_REPLACED;
            currently_saved_prefix++;
        } else {
            return currently_saved_prefix;
        }
    }
    if (segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].status == RECEIVED_DATA){
        fwrite(segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].buffer, sizeof(char), size % BUFFER_SIZE, file);
        segments[(size/BUFFER_SIZE) % SEGMENTS_LEN].status = CAN_BE_REPLACED;
        currently_saved_prefix++;
    }
    return currently_saved_prefix;
}
