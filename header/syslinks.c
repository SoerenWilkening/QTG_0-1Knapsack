/* 
 * =============================================================================
 *                            OS-independent includes
 * =============================================================================
 */

#include "syslinks.h"

#if defined(_WIN32) || defined(_WIN64)

/* 
 * =============================================================================
 *                            Windows: includes
 * =============================================================================
 */

#include <windows.h>
#include <direct.h>
#include <intrin.h>

/* 
 * =============================================================================
 *                            Windows: path separator
 * =============================================================================
 */

uint8_t
path_sep() {
	return '\\';
}

/* 
 * =============================================================================
 *                            Windows: file exists
 * =============================================================================
 */

uint8_t
file_exists(const char* filename) {
    DWORD file_att = GetFileAttributes(filename);
    return file_att != INVALID_FILE_ATTRIBUTES && \
    	   !(file_att & FILE_ATTRIBUTE_DIRECTORY);
}

/* 
 * =============================================================================
 *                            Windows: create directory
 * =============================================================================
 */

uint8_t
create_dir(const char* dirname) {
	return _mkdir(dirname);
}

/* 
 * =============================================================================
 *                            Windows: read time-stamp counter
 * =============================================================================
 */

uint64_t
rdtsc() {
    return _rdtsc();
}

#elif defined(__unix__)

/* 
 * =============================================================================
 *                            Unix: includes
 * =============================================================================
 */

#include <unistd.h>
#include <sys/stat.h>

/* 
 * =============================================================================
 *                            Unix: path separator
 * =============================================================================
 */

uint8_t
path_sep() {
	return '/';
}

/* 
 * =============================================================================
 *                            Unix: file exists
 * =============================================================================
 */

uint8_t
file_exists(const char* filename) {
	return !access(filename, F_OK);
}

/* 
 * =============================================================================
 *                            Unix: create directory
 * =============================================================================
 */

uint8_t
create_dir(const char* dirname) {
	return !mkdir(dirname, 0777);
}

/* 
 * =============================================================================
 *                            Unix: read time-stamp counter
 * =============================================================================
 */

uint64_t
rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
    	"rdtsc" : "=a" (lo), "=d" (hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

#elif defined(__APPLE__)

/* 
 * =============================================================================
 *                            Apple: includes
 * =============================================================================
 */

#include <unistd.h>
#include <sys/stat.h>
#include <mach/task.h>
#include <mach/mach_init.h>
#include <mach/vm_map.h> 

/* 
 * =============================================================================
 *                            Apple: path separator
 * =============================================================================
 */

uint8_t
path_sep() {
    return '/';
}

/* 
 * =============================================================================
 *                            Apple: file exists
 * =============================================================================
 */

uint8_t
file_exists(const char* filename) {
    return !access(filename, F_OK);
}

/* 
 * =============================================================================
 *                            Apple: create directory
 * =============================================================================
 */

uint8_t
create_dir(const char* dirname) {
    return !mkdir(dirname, 0777);
}

/* 
 * =============================================================================
 *                            Apple: read time-stamp counter
 * =============================================================================
 */

uint64_t
rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
        "rdtsc" : "=a" (lo), "=d" (hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

/* 
 * =============================================================================
 *                            Apple: read meta data
 * =============================================================================
 */

void
rdmd(const char* executable, size_t argc, char* argv[], uint64_t* mem_count, \
     uint64_t* cycle_count) {
    char arg_str[256];
    size_t offset = 0;
    for (size_t i = 0; i < argc; ++i) {
        offset += snprintf(arg_str + offset, sizeof(arg_str) - offset, "%s ", \
                           argv[i]);
    }

    char command[512];
    snprintf(command, sizeof(command), "echo \"$(/usr/bin/time -l ./%s %s" \
             "2>&1)\" | grep \"maximum resident set size\\|cycles elapsed\"", \
             executable, arg_str);

    FILE* pipe = popen(command, "r");
    if (!pipe) {
        printf("Error opening pipe!\n");
        return;
    }

    /* get peak memory usage */
    char mem_buffer[256];
    fgets(mem_buffer, sizeof(mem_buffer), pipe);
    *mem_count = strtoull(mem_buffer, NULL, 10);

    /* get cycle count */
    char cycle_buffer[256];
    fgets(cycle_buffer, sizeof(cycle_buffer), pipe);
    *cycle_count = strtoull(cycle_buffer, NULL, 10);


    pclose(pipe);
}

#endif