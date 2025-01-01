#include "grv/grv.h"
#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>

char* get_mem_color(int percent_used) {
    return percent_used >= 90 ? COLOR_RED : percent_used >= 70 ? COLOR_YELLOW : COLOR_GREEN;
}

char* format_memory_usage(void) {
    struct sysinfo info;
    sysinfo(&info);
    unsigned long mb = 1024 * 1024;
    long free_ram = (int)(info.freeram / mb);
    long buffer_ram = (int)(info.bufferram / mb);
    long total_ram = (int)(info.totalram / mb);
    long total_swp = (int)(info.totalswap / mb);
    long free_swp = (int)(info.freeswap / mb);

    float ram_used = 1.0f - (float)(free_ram + buffer_ram) / total_ram;
    float swp_used = 1.0f - (float)free_swp / total_swp;

    grv_str_t ram_str = grv_str_format(grv_str_ref("MEM {f32:.02}"), ram_used);
    grv_str_t swp_str = grv_str_format(grv_str_ref("SWP {f32:.02}"), swp_used);

    char* ram = formatStatusField(grv_str_cstr(ram_str), get_mem_color(ram_used));
    char* swp = formatStatusField(grv_str_cstr(swp_str), get_mem_color(swp_used));

    char* result = grv_cstr_append(ram, ",");
    result = grv_cstr_append(result, swp);

    grv_str_free(&ram_str);
    grv_str_free(&swp_str);
    free(swp);
    
    return result;
}
