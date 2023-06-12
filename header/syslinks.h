#ifndef SYSLINKS_H
#define SYSLINKS_H

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include <stdint.h>

/* 
 * =============================================================================
 *                            C++ check
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * =============================================================================
 *                            path separator
 * =============================================================================
 */

/*
 * Function:    path_sep()
 * -----------------------
 * Description: This function returns the OS-specific path separator character.
 * Returns:     The OS-specific path separator character.
 */
uint8_t path_sep();

/* 
 * =============================================================================
 *                            file exists
 * =============================================================================
 */

/*
 * Function:    file_exists
 * ------------------------
 * Description: This function checks whether the given file exists.
 * Parameter:	Path of the file that should be checked for.
 * Returns:     Whether the file exists or not.
 */
uint8_t file_exists(const char*);

/* 
 * =============================================================================
 *                            create directory
 * =============================================================================
 */

/*
 * Function:    create_dir
 * -----------------------
 * Description: This function attempts to create the specified directory.
 * Parameter:	Path of the directory that should be created.
 * Returns:     Whether creation of the directory was successful.
 */
uint8_t create_dir(const char*);

/* 
 * =============================================================================
 *                            read time-stamp counter
 * =============================================================================
 */

/*
 * Function:    rdtsc
 * ------------------
 * Description: This function returns reads the system's current time-stamp
 *				counter.
 * Returns:     The system's current time-stamp counter.
 */
uint64_t rdtsc();

#ifdef __cplusplus
}
#endif

#endif
