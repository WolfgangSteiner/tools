#include "grv/grv.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

static int save_percent = 20;
static int danger_percent = 10;
static int critical_percent = 5;

bool is_battery_discharging(void) {
    grv_str_return_t res = grv_read_file(grv_str_ref("/sys/class/power_supply/BAT0/status"));
    grv_str_t status = res.str;
    bool discharging = grv_str_starts_with_cstr(status, "Discharging");
    grv_str_free(&status);
    return discharging;
}

int get_capacity(void) {
    grv_str_return_t res = grv_read_file(grv_str_ref("/sys/class/power_supply/BAT0/capacity"));
    grv_str_t capacity_str = res.str;
    int capacity = grv_str_to_int(capacity_str);
    printf("capacity: %d\n", capacity);
    grv_str_free(&capacity_str);
    return capacity;
}

bool is_process_running(pid_t pid) {
    int result = kill(pid, 0);
    if (result == -1 && errno == ESRCH) {
        return false;
    } else {
        return true;
    }   
}

pid_t update_nagbar(char* type, pid_t nagbar_pid, int capacity) {
    static int prev_capacity = -1;

    if (capacity != prev_capacity) {
        // If the nagbar is already running, kill it.
        if (nagbar_pid != -1) {
            kill(nagbar_pid, SIGTERM);
            nagbar_pid = -1;
            prev_capacity = -1;
        }

        // show the updated nagbar
        pid_t pid = fork();
        if (pid == 0) {
            char* message = grv_cstr_new_with_format("  Battery is at %d%%", capacity);
            system("i3-msg fullscreen disable");
            execlp("i3-nagbar", "i3-nagbar",
                    "-t", type,
                    "-m", message,
                    "-b", " Hibernate ", "systemctl hybrid-sleep",
                    "-f", "pango:DejaVu Sans Mono Bold 14",
                    NULL);
        } else {
            nagbar_pid = pid;
            prev_capacity = capacity;
        }
    }

    return nagbar_pid;
}

int main(void) {
    pid_t nagbar_pid = -1;
    while (true) {
        if (is_battery_discharging()) {
            int capacity = get_capacity();
            if (capacity < critical_percent) {
                system("systemctl hybrid-sleep");
            } else if (capacity < danger_percent) {
                nagbar_pid = update_nagbar("error", nagbar_pid, capacity);
            } else if (capacity < save_percent) {
                nagbar_pid = update_nagbar("warning", nagbar_pid, capacity);
            } else {
                if (nagbar_pid != -1) {
                    kill(nagbar_pid, SIGTERM);
                    nagbar_pid = -1;
                }
            }
        } else if (nagbar_pid != -1) {
            kill(nagbar_pid, SIGTERM);
            nagbar_pid = -1;
        }

        sleep(5);
    }

    return 0;
}
