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
 *                            Apple: read peak memory usage
 * =============================================================================
 */

uint64_t
rpmu(funptr fun, void* args) {
    task_vm_info_data_t vm_info;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;

    // Get initial memory usage
    kern_return_t result = task_info(mach_task_self(),
                                     TASK_VM_INFO,
                                     (task_info_t)&vm_info,
                                     &count);
    if (result != KERN_SUCCESS) {
        return 0;
    }

    // Call the given function
    fun(args);

    // Allow some time for the memory usage to stabilize
    //usleep(1000);

    // Get final memory usage
    result = task_info(mach_task_self(),
                       TASK_VM_INFO,
                       (task_info_t)&vm_info,
                       &count);
    if (result != KERN_SUCCESS) {
        return 0;
    }

    // Calculate peak memory usage
    vm_size_t peak_memory = vm_info.phys_footprint;
    return peak_memory;
}

#endif