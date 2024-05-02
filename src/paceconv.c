#include <stdio.h>
#include "grv/grv.h"
#include "grv/grv_str.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Missing min/mile pace");
        exit(1);
    } else {
        for (int i = 1; i < argc; ++i) {
            grv_str_t min_per_mile = grv_str_ref(argv[i]);
            grv_str_t sep = grv_str_ref(":");
            grv_strpair_t pair = grv_str_split_head_front(min_per_mile, sep);
            int sec_per_mile = grv_str_to_int(pair.second) + grv_str_to_int(pair.first) * 60;
            int sec_per_k = (int)(sec_per_mile / 1.609f + 0.5f);
            int pace_per_k_min = sec_per_k / 60;
            int pace_per_k_sec = sec_per_k % 60;
            printf("%s min/mile --> %d:%02d min/km\n", argv[i], pace_per_k_min, pace_per_k_sec);
        }
    }
}
