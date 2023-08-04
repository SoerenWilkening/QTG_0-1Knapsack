#ifndef SYSLINKS_H
#define SYSLINKS_H

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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
 * Description: This function returns the system's current time-stamp counter.
 * Returns:     The system's current time-stamp counter.
 */
uint64_t rdtsc();

/* 
 * =============================================================================
 *                            read meta data
 * =============================================================================
 */

/*
 * Function:    rdmd
 * -----------------
 * Description:     This function calculates the peak memory usage of a given
 *                  function that accepts a given void pointer as argument.
 * Parameters:
 *      parameter1: Pointer to function whose peak memory usage should be
 *                  estimated.
 *      parameter2: Void pointer to arguments the measured function takes.
 * Returns:         Peak memory usage of the given function.
 */
void rdmd(const char*, size_t, char*[], uint64_t*, uint64_t*);

#ifdef __cplusplus
}
#endif

#endif
