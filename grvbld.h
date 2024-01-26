#ifndef GRVBLD_H
#define GRVBLD_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef NULL
#define NULL ((void*)0L);
#endif

#ifdef _WIN32
#define GRVBLD_PATH_SEP '\\'
#include <direct.h>
#else
#define GRVBLD_PATH_SEP '/'
#include <dirent.h>
#include <sys/stat.h>
#endif

#define GRVBLD_INLINE static inline 

//==============================================================================
// pass-through of cmd line arguments
//==============================================================================
GRVBLD_INLINE char* concatenate_arguments(int argc, char** argv) {
    int size = strlen(argv[0]) + 1;
    char* cmd_line = calloc(size, 1);
    strcat(cmd_line, argv[0]);  
    for (int i = 1; i < argc; ++i) {
        size += strlen(argv[i]) + 1;
        cmd_line = realloc(cmd_line, size);
        strcat(cmd_line, " ");
        strcat(cmd_line, argv[i]);
    }
    return cmd_line;
}

//==============================================================================
// default main function
//==============================================================================
#ifndef GRV_BUILD_CONFIGURED
int main(int argc, char** argv) {
    int result = system("gcc -g -o bld -DGRV_BUILD_CONFIGURED bld.c");
    if (result != 0) {
        printf("[ERROR] Failed to build bld.c\n");
            return result;
    }
    system(concatenate_arguments(argc, argv));
}
#endif


//==============================================================================
// grvbld_cstr
//==============================================================================
GRVBLD_INLINE char* grvbld_cstr_new(char* str) {
    size_t len = strlen(str);
    char* new_str = malloc(len + 1);
    memcpy(new_str, str, len + 1);
    return new_str;
}

GRVBLD_INLINE char* grvbld_cstr_append(char* str, char* append_str) {
    size_t len = strlen(str);
    size_t append_len = strlen(append_str);
    str = realloc(str, len + append_len + 1);
    memcpy(str + len, append_str, append_len + 1);
    return str;
}

GRVBLD_INLINE char* grvbld_cstr_cat(char* a, char* b) {
    char* res = grvbld_cstr_new(a);
    res = grvbld_cstr_append(res, b);
    return res;
}

GRVBLD_INLINE char* grvbld_cstr_cat3(char* a, char* b, char* c) {
    char* res = grvbld_cstr_new(a);
    res = grvbld_cstr_append(res, b);
    res = grvbld_cstr_append(res, c);
    return res;
}   


//==============================================================================
// File system
//==============================================================================

GRVBLD_INLINE int grvbld_modtime(char* filename) {
    struct stat attr;
    stat(filename, &attr);
    return attr.st_mtime;
}

GRVBLD_INLINE bool grvbld_is_file_newer_than(char* filename, char* other_filename) {
    int mod_time_a = grvbld_modtime(filename);
    int mod_time_b = grvbld_modtime(other_filename);
    return mod_time_a > mod_time_b;
}

GRVBLD_INLINE bool is_source_file_newer(char* exe_filename) {
    char* src_filename = grvbld_cstr_cat3("src/", exe_filename, ".c");
    bool result = grvbld_is_file_newer_than(src_filename, exe_filename);
    result = result || grvbld_is_file_newer_than("src/grvbld.c", exe_filename);
    result = result || grvbld_is_file_newer_than("include/grv/grvbld.h", exe_filename);
    free(src_filename);
    return result;
}


//==============================================================================
// Logging
//==============================================================================
__attribute__((format(__printf__, 1, 2)))
GRVBLD_INLINE void log_info(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("[INFO] ");
    vprintf(fmt, args);
    printf("\n");
}

__attribute__((format(__printf__, 1, 2)))
GRVBLD_INLINE void log_error(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("[ERROR] ");
    vprintf(fmt, args);
    printf("\n");
}

GRVBLD_INLINE int rebuild_file(char* filename) {
    char* src = grvbld_cstr_cat3("src/", filename, ".c");
    char* cmd = grvbld_cstr_cat("gcc -o ", filename);
    cmd = grvbld_cstr_append(cmd, " -DGRV_BUILD_CONFIGURED -Iinclude ");
    cmd = grvbld_cstr_append(cmd, src);
    cmd = grvbld_cstr_append(cmd, " src/grvbld.c");
    log_info("Rebuilding %s:", filename);
    log_info("%s", cmd);
    int result = system(cmd);
    free(src);
    free(cmd);
    return result;
}

#define GRV_CHECK_AND_REBUILD() \
    char* _executable_name = argv[0]; \
    if (is_source_file_newer(_executable_name)) { \
        int result = rebuild_file(_executable_name); \
        if (result == 0) { \
            log_info("Rebuilding of %s successful.", _executable_name); \
            system(_executable_name); \
            return 1; \
        } else { \
            log_error("Failed to rebuild %s", _executable_name); \
            return result; \
        } \
    }

//==============================================================================
// Data types
//==============================================================================
typedef struct {
    char** data;
    size_t size;
    size_t capacity;
} grvbld_strarr_t;


typedef enum {
    GRVBLD_EXECUTABLE,
    GRVBLD_STATIC_LIBRARY
} grvbld_target_type_t;


struct grvbld_target_t;

typedef struct {
    struct grvbld_target_t** arr;
    size_t size;
    size_t capacity;
} grvbld_target_arr_t;

typedef struct grvbld_target_t{
    char* name;
    grvbld_target_type_t type;
    grvbld_strarr_t src_files;
    grvbld_strarr_t libs;
    grvbld_target_arr_t linked_targets; 
    bool run_after_build;
} grvbld_target_t;

typedef struct {
    char* cc;
    char* std;
    grvbld_strarr_t inc;
    grvbld_strarr_t warnings;
    grvbld_strarr_t libs;
    grvbld_strarr_t library_dirs;
    grvbld_strarr_t defines;
    char* build_dir;
    char* test_dir;
    bool debug;
    bool treat_warnings_as_errors;
    bool use_ccache;
    int verbosity;
} grvbld_config_t;


//==============================================================================
// grvbld_strarr
//==============================================================================
GRVBLD_INLINE void grvbld_strarr_init(grvbld_strarr_t* arr) {
    arr->size = 0;
    arr->capacity = 16;
    arr->data = calloc(arr->capacity, sizeof(char*));
}

GRVBLD_INLINE void grvbld_strarr_push(grvbld_strarr_t* arr, char* str) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = realloc(arr->data, arr->capacity * sizeof(char*));
    }
    arr->data[arr->size++] = grvbld_cstr_new(str);
}

GRVBLD_INLINE void grvbld_strarr_dup(grvbld_strarr_t* dst, grvbld_strarr_t* src) {
    dst->size = src->size;
    dst->capacity = src->capacity;
    dst->data = calloc(dst->capacity, sizeof(char*));
    for (size_t i = 0; i < src->size; ++i) {
        dst->data[i] = grvbld_cstr_new(src->data[i]);
    }
}


//==============================================================================
// grvbld_str 
//==============================================================================

GRVBLD_INLINE size_t grvbld_cstr_len(char* str) {
    return strlen(str);
}

GRVBLD_INLINE char* grvbld_cstr_append_char(char* dst, char c) {
    size_t len = grvbld_cstr_len(dst);
    dst = realloc(dst, len + 2);
    dst[len] = c;
    dst[len + 1] = '\0';
    return dst;
}

GRVBLD_INLINE char* grvbld_cstr_append_path(char* dst, char* path) {
    dst = grvbld_cstr_append_char(dst, GRVBLD_PATH_SEP);
    dst = grvbld_cstr_append(dst, path);
    return dst;
}

GRVBLD_INLINE char* grvbld_cstr_append_arg(char* dst, char* arg) {
    dst = grvbld_cstr_append(dst, " ");
    dst = grvbld_cstr_append(dst, arg);
    return dst;
}

GRVBLD_INLINE char* grvbld_cstr_filename(char* str) {
    char* last_sep = strrchr(str, GRVBLD_PATH_SEP);
    if (last_sep) {
        return grvbld_cstr_new(last_sep + 1);
    } else {
        return grvbld_cstr_new(str);
    } 
}

GRVBLD_INLINE void grvbld_cstr_remove_ext(char* str) {
    char* dot = strrchr(str, '.');
    if (dot) {
        *dot = 0;
    }
}

GRVBLD_INLINE char* grvbld_cstr_replace_ext(char* str, char* ext) {
    grvbld_cstr_remove_ext(str);
    str = grvbld_cstr_append(str, ext);
    return str;
 }

GRVBLD_INLINE char* grvbld_cstr_prepend_path(char* str, char* path) {
    char* result = grvbld_cstr_new(path);
    result = grvbld_cstr_append_path(result, str);
    return result;
}

GRVBLD_INLINE char* grvbld_cstr_new_with_format(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* result = calloc(len + 1, sizeof(char));
    va_start(args, fmt);
    vsnprintf(result, len + 1, fmt, args);
    va_end(args);
    return result;
}

GRVBLD_INLINE char* grvbld_cstr_append_arg_format(char* dst, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* formatted_str = calloc(len + 1, sizeof(char));
    va_start(args, fmt);
    vsnprintf(formatted_str, len + 1, fmt, args);
    va_end(args);
    dst = grvbld_cstr_append_arg(dst, formatted_str);
    free(formatted_str);
    return dst;
} 

GRVBLD_INLINE bool starts_with(char* str, char* prefix) {
    size_t prefix_len = strlen(prefix);
    return strncmp(str, prefix, prefix_len) == 0;
}

GRVBLD_INLINE bool ends_with(char* str, char* suffix) {
    size_t suffix_len = strlen(suffix);
    size_t str_len = strlen(str);
    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

GRVBLD_INLINE bool grvbld_cstr_eq(char* a, char* b) {
    return strcmp(a, b) == 0;
}

GRVBLD_INLINE bool grvbld_args_contain(int argc, char** argv, char* arg) {
    for (int i = 0; i < argc; ++i) {
        if (grvbld_cstr_eq(argv[i], arg)) {
            return true;
        }
    }
    return false;
}

//==============================================================================
// grvbld_config_t
//==============================================================================
GRVBLD_INLINE grvbld_config_t* grvbld_config_new(int argc, char** argv) {
    grvbld_config_t* config = calloc(1, sizeof(grvbld_config_t));
    config->cc = "gcc";
    config->std = "gnu11";
    grvbld_strarr_init(&config->inc);
    grvbld_strarr_init(&config->warnings);
    grvbld_strarr_init(&config->libs);
    grvbld_strarr_init(&config->library_dirs);
    grvbld_strarr_init(&config->defines);
    config->build_dir = "build";
    config->test_dir = "test";
    config->debug = true;
    config->treat_warnings_as_errors = false;
    config->verbosity = 0;
    config->use_ccache = true;

    grvbld_strarr_push(&config->inc, "include");
    grvbld_strarr_push(&config->warnings, "-Wall");
    ///grvbld_strarr_push(&config->warnings, "-Wextra");
    //grvbld_strarr_push(&config->warnings, "-Wpedantic");
    grvbld_strarr_push(&config->warnings, "-Werror=implicit-function-declaration");


    grvbld_strarr_push(&config->libs, "-lm");

    if (grvbld_args_contain(argc, argv, "--debug")) config->debug = true;
    if (grvbld_args_contain(argc, argv, "--use-ccache")) config->use_ccache = true;

    return config;
}


GRVBLD_INLINE grvbld_config_t* grvbld_config_dup(grvbld_config_t* src) {
    grvbld_config_t* dst = calloc(1, sizeof(grvbld_config_t));
    memcpy(dst, src, sizeof(grvbld_config_t));
    dst->cc = grvbld_cstr_new(src->cc);
    dst->std = grvbld_cstr_new(src->std);
    grvbld_strarr_dup(&dst->inc, &src->inc); 
    grvbld_strarr_dup(&dst->warnings, &src->warnings);
    grvbld_strarr_dup(&dst->libs, &src->libs);
    grvbld_strarr_dup(&dst->defines, &src->defines);
    dst->build_dir = grvbld_cstr_new(src->build_dir);
    dst->test_dir = grvbld_cstr_new(src->test_dir);
    return dst;
}

GRVBLD_INLINE void grvbld_config_add_include_directory(grvbld_config_t* config, char* path) {
    grvbld_strarr_push(&config->inc, path);
}

//==============================================================================
// utility functions
//==============================================================================
GRVBLD_INLINE bool has_directory(char* path) {
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
#endif
}

GRVBLD_INLINE bool create_directory(char* path) {
#ifdef _WIN32
    return CreateDirectoryA(path, NULL);
#else
    return mkdir(path, 0777) == 0;
#endif
}

GRVBLD_INLINE bool make_path(char* path) {
    bool result = false;
    char* p = grvbld_cstr_new(path);
    char* s = p;
    char* e = strchr(s, GRVBLD_PATH_SEP);    
    while (e) {
        *e = 0;
        if (!has_directory(path)) {
            if (!create_directory(path)) {
                goto end;
            }
        }
        s = e + 1;
        e = strchr(s, GRVBLD_PATH_SEP);
    }   

    if (!has_directory(s)) {
        if (!create_directory(s)) {
            goto end;
        }
    }

    result = true;

end:
    free(p);
    return result;
}

GRVBLD_INLINE grvbld_strarr_t* get_files_in_dir(char* path) {
    if (!has_directory(path)) {
        printf("[ERROR] path \"%s\" does not exist\n", path);
        exit(1);
    }

    grvbld_strarr_t* result = calloc(1, sizeof(grvbld_strarr_t));
    grvbld_strarr_init(result);


#ifdef _WIN32
    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA(path, &find_data);
    if (find_handle == INVALID_HANDLE_VALUE) {
        return result;
    }

    do {
        grvbld_strarr_push(result, find_data.cFileName);
    } while (FindNextFileA(find_handle, &find_data));

    FindClose(find_handle);

#else
    DIR* directory = opendir(path);
    if (directory == NULL) {
        return result;
    }

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        grvbld_strarr_push(result, entry->d_name);
    }

    closedir(directory);
#endif
    return result;
}

//==============================================================================
// grvbld_target_t
//==============================================================================
GRVBLD_INLINE grvbld_target_t* grvbld_target_create(char* name, grvbld_target_type_t type) {
    grvbld_target_t* target = calloc(1, sizeof(grvbld_target_t));
    target->name = grvbld_cstr_new(name);
    target->type = type;
    grvbld_strarr_init(&target->src_files);
    grvbld_strarr_init(&target->libs);
    target->linked_targets = (grvbld_target_arr_t){0};
    return target;
}

GRVBLD_INLINE void grvbld_target_add_src(grvbld_target_t* target, char* src) {
    grvbld_strarr_push(&target->src_files, src);
}

GRVBLD_INLINE void grvbld_target_link_library(grvbld_target_t* target, char* lib) {
    grvbld_strarr_push(&target->libs, lib);
}

GRVBLD_INLINE void grvbld_target_link(grvbld_target_t* target, grvbld_target_t* linked_target) {
    grvbld_target_arr_t* arr = &target->linked_targets;
    if (arr->capacity == 0) {
        arr->capacity = 16;
        arr->arr = calloc(arr->capacity, sizeof(grvbld_target_t*));
    } else if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->arr = realloc(arr->arr, arr->capacity * sizeof(grvbld_target_t*));
    }
    arr->arr[arr->size++] = linked_target;
}

//==============================================================================
// grvbld main functions
//==============================================================================
GRVBLD_INLINE void create_build_path(grvbld_config_t* config) {
    bool make_path_ok = make_path(config->build_dir);
    if (!make_path_ok) {
        printf("[ERROR] failed to create path \"%s\"\n", config->build_dir);
        exit(1);
    }
}

GRVBLD_INLINE char* grvbld_build_cmd(grvbld_config_t* config) {
    char* cmd = grvbld_cstr_new("");
    if (config->use_ccache) {
        cmd = grvbld_cstr_append(cmd, "ccache ");
    }
    cmd = grvbld_cstr_append(cmd, config->cc);
    cmd = grvbld_cstr_append_arg_format(cmd, "-std=%s", config->std);
    
    for (size_t i = 0; i < config->inc.size; ++i) {
        cmd = grvbld_cstr_append_arg_format(cmd, "-I%s", config->inc.data[i]);
    }

    for (size_t i = 0; i < config->warnings.size; ++i) {
        cmd = grvbld_cstr_append_arg(cmd, config->warnings.data[i]);
    }

    if (config->debug) {
        cmd = grvbld_cstr_append_arg(cmd, "-g");
    }

    if (config->treat_warnings_as_errors) {
        cmd = grvbld_cstr_append_arg(cmd, "-Werror");
    }

    for (size_t i = 0; i < config->libs.size; ++i) {
        cmd = grvbld_cstr_append_arg(cmd, config->libs.data[i]);
    }

    for (size_t i = 0; i < config->defines.size; ++i) {
        cmd = grvbld_cstr_append_arg_format(cmd, "-D%s", config->defines.data[i]);
    }

//    if (config->debug) {
//        cmd = grvbld_cstr_append_arg(cmd, "-DGRV_DEBUG_MEMORY");
//    }

    cmd = grvbld_cstr_append_arg_format(cmd, "-L%s", config->build_dir);

    return cmd;
}

GRVBLD_INLINE int grvbld_test(grvbld_config_t* config, char* name) {
    config = grvbld_config_dup(config);    
    config->debug = true;
    //grvbld_strarr_push(&config->defines, "GRV_DEBUG_MEMORY");

    char* dst_dir = grvbld_cstr_new_with_format("%s/test", config->build_dir);
    bool make_path_ok = make_path(dst_dir);
    if (!make_path_ok) {
        log_error("failed to create path \"%s\"", dst_dir);
        exit(1);
    }

    char* src_file = grvbld_cstr_new_with_format("%s/%s.c", config->test_dir, name);
    char* dst_file = grvbld_cstr_new_with_format("%s/test/%s", config->build_dir, name);
    char* cmd = grvbld_build_cmd(config);
    
    cmd = grvbld_cstr_append_arg_format(cmd, "-o %s", dst_file);
    cmd = grvbld_cstr_append_arg(cmd, src_file);

    cmd = grvbld_cstr_append_arg(cmd, "-lgrv");
    cmd = grvbld_cstr_append_arg(cmd, "-lm");
    // cmd = grvbld_cstr_append_arg(cmd, "src/grv.c");

    if (config->verbosity > 0) {
        log_info("%s", cmd);
    }
    
    int result = system(cmd);
    if (result != 0) {
        log_error("failed to build test \"%s\"", name);
        exit(1);
    }

    system(dst_file);
    
    #if 0
    if (grv_cmd_available(grv_str_ref("valgrind"))) {
        grvbld_cstr_t* valgrind_cmd = grvbld_str_alloc("valgrind --leak-check=full --show-leak-kinds=all ");
        grvbld_cstr_append_arg(valgrind_cmd, dst_file->str);
        printf("[INFO] %s\n", valgrind_cmd->str);
        system(valgrind_cmd->str);
    }
    #endif
}

GRVBLD_INLINE int grvbld_run_tests(grvbld_config_t* config) {
    grvbld_strarr_t* test_files = get_files_in_dir(config->test_dir);
    for (size_t i = 0; i < test_files->size; ++i) {
        char* test_file = test_files->data[i];
        if (starts_with(test_file, "test_")
            && ends_with(test_file, ".c")) {
            grvbld_cstr_remove_ext(test_file);
            grvbld_test(config, test_file);
        }
    }
}


GRVBLD_INLINE int grvbld_build_static_library(grvbld_config_t* config, grvbld_target_t* target) {
    char* lib_file = grvbld_cstr_new_with_format("%s/lib%s.a", config->build_dir, target->name);
    char* ar_cmd = grvbld_cstr_new_with_format("rm -f %s && ar rcs %s", lib_file, lib_file);

    char* cmd = grvbld_build_cmd(config);
    grvbld_cstr_append_arg(cmd, "-c");

    // compile all of the source files
    for (size_t i = 0; i < target->src_files.size; ++i) {
        char* src_file = grvbld_cstr_new(target->src_files.data[i]);
        char* obj_file = grvbld_cstr_filename(src_file);
        obj_file = grvbld_cstr_prepend_path(obj_file, config->build_dir);
        obj_file = grvbld_cstr_replace_ext(obj_file, ".o");
        cmd = grvbld_cstr_append_arg(cmd, src_file);
        cmd = grvbld_cstr_append_arg_format(cmd, "-o %s", obj_file);
        cmd = grvbld_cstr_append_arg(cmd, "-c");
        ar_cmd = grvbld_cstr_append_arg(ar_cmd, obj_file);

        log_info("%s", cmd);
        int result = system(cmd);
        if (result != 0) {
            log_error("failed to build %s", target->name);
            exit(1);
        }
    }

    log_info("%s", ar_cmd);
    int result = system(ar_cmd);
    if (result != 0) {
        log_error("failed to build %s", target->name);
        exit(1);
    }
}

GRVBLD_INLINE int grvbld_build_target(grvbld_config_t* config, grvbld_target_t* target) {
    create_build_path(config);

    char* cmd = grvbld_build_cmd(config);
    
    if (target->type == GRVBLD_STATIC_LIBRARY) {
        grvbld_build_static_library(config, target);
    } else if (target->type == GRVBLD_EXECUTABLE) {
        char* dst_file = grvbld_cstr_new_with_format("%s/%s", config->build_dir, target->name);
        cmd = grvbld_cstr_append_arg_format(cmd, "-o %s", dst_file);

        for (size_t i = 0; i < target->src_files.size; ++i) {
            cmd = grvbld_cstr_append_arg(cmd, target->src_files.data[i]);
        }
    
        for (size_t i = 0; i < target->libs.size; ++i) {
            cmd = grvbld_cstr_append_arg_format(cmd, "-l%s", target->libs.data[i]);
        }

        for (size_t i = 0; i < target->linked_targets.size; ++i) {
            grvbld_target_t* linked_target = target->linked_targets.arr[i];
            if (linked_target->type == GRVBLD_STATIC_LIBRARY) {
                cmd = grvbld_cstr_append_arg_format(cmd, "-l%s", linked_target->name);
                for (size_t j = 0; j < linked_target->libs.size; ++j) {
                    cmd = grvbld_cstr_append_arg_format(cmd, "-l%s", linked_target->libs.data[j]);
                }
            }
        }        

        cmd = grvbld_cstr_append_arg(cmd, "-lm");
        log_info("%s", cmd);
        int result = system(cmd);
        if (result == 0 && target->run_after_build) {
            char* debug_cmd = grvbld_cstr_new_with_format("gdb -q -ex run -ex quit %s", dst_file);
            system(debug_cmd);
        }
    }
}



#endif