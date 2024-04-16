#include "grv/grv.h"
#include "grv/grv_common.h"
#include "grv/grv_str.h"
#include "grv/grv_strarr.h"
#include "grv/grv_util.h"

u8 random_u8(u8 max_val) {
    u32 num_values = max_val + 1;
    u32 num_values_u8 = 256;
    u32 multiplier = num_values_u8 / num_values;
    u32 rand_value = grv_random_u8();
    while (rand_value >= num_values * multiplier) {
        rand_value = grv_random_u8();
    }
    return rand_value % num_values;
}

grv_str_t generate_password(int word_count) {
    grv_str_t pwd = {0};
    char* consonants = "bcdfghjklmnpqrstvwxyz";
    int num_consonants = strlen(consonants);
    char* vocals = "aeiou";
    int num_vocals = strlen(vocals);
    int word_len = 6;
    for (int i = 0; i < word_count; ++i) {
        if (i > 0) grv_str_append_char(&pwd,'-');
        for (int j = 0; j < word_len / 2; ++j) {
            char c = consonants[random_u8(num_consonants - 1)];
            char v = vocals[random_u8(num_vocals - 1)];
            grv_str_append_char(&pwd, c);
            grv_str_append_char(&pwd, v);
        }
    }
    return pwd;
}

void print_help(void) {
    printf("pwgen: generate pronounceable passwords.\n");
    printf("usage: pwgen [-n num_passwords] [--medium | --standard | --strong]\n");
    printf("options:\n");
    printf("-n num_passwords  number of passwords to generate\n");
    printf("--medium          generate passwords of medium strength (~60 bits of entropy)\n");
    printf("--standard        generate passwords of standard strength (~80 bits of entropy)\n");
    printf("--strong          generate strong passwords (~120 bits of entropy)\n");
}

int main(int argc, char** argv) {
    grv_strarr_t args = grv_strarr_new_from_cstrarr(argv+1, argc-1);
    int word_count = 4;
    int pwd_count = 1;

    while (args.size) {
        grv_str_t arg = grv_strarr_pop_front(&args);
        if (grv_str_eq(arg, "--medium")) {
            word_count = 3; // ~60 bits of entropy 
        } else if (grv_str_eq(arg, "--standard")) {
            word_count = 4; // ~80 bits of entropy
        } else if (grv_str_eq(arg, "--strong")) {
            word_count = 6; // ~120 bits of entropy
        } else if (grv_str_eq(arg, "-n")) {
            if (arg.size == 0 || !grv_str_is_int(*grv_strarr_front(args))) {
                grv_log_error(grv_str_ref("Please provide number of passwords for option -n"));
                exit(1);
            }
            grv_str_t count_str = grv_strarr_pop_front(&args);
            pwd_count = grv_str_to_int(count_str);
        } else if (grv_str_eq(arg, "--help") || grv_str_eq(arg, "-h")) {
            print_help();
            exit(0);
        } else {
            grv_str_t error_msg = grv_str_format(grv_str_ref("Invalid option {str}"), arg);
            grv_log_error(error_msg);
            exit(1);
        }
    }

    for (int i = 0; i < pwd_count; ++i) {
         grv_str_t pwd = generate_password(word_count);
         grv_str_print(pwd);
    }
}

