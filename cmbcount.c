/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#if defined(_WIN32) || defined(_WIN64)
    #include "include\knapsack.h"
    #include "include\combowrp.h"
#else
    #include "include/knapsack.h"
    #include "include/combowrp.h"
#endif

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
 *                            type definitions
 * =============================================================================
 */

typedef enum ins {
    JOOKEN,
    PISINGER,
} ins_t;

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

/* 
 * =============================================================================
 *                            main
 * =============================================================================
 */

int main(int argc, char* argv[]){

    /* determine which type of knapsack should be created */
    ins_t instance;
    if (*(argv[1]) == 'p') {
        instance = JOOKEN;
    } else if (*(argv[1]) == 's' || *(argv[1]) == 'l' || *(argv[1]) == 'h') {
        instance = PISINGER;
    }

    char filename[256];
    size_t offset = 0;
    offset += snprintf(filename, sizeof(filename), "instances%c%s", \
                       path_sep(), argv[1]);

    knapsack_t* k;
    switch (instance) {
        case JOOKEN: {
            snprintf(filename + offset, sizeof(filename) + offset, \
                     "%ctest.in", path_sep());
            k = create_jooken_knapsack(filename);
            break;
        }

        case PISINGER: {
            snprintf(filename + offset, sizeof(filename) + offset, ".csv");
            k = create_pisinger_knapsack(filename);
            break;
        }

        default: {
            printf("Unspecified instance type!\n");
            return -1;
        }
    }

    /* determine whether Combo should find the solution vector as well */
    bool_t def;
    def = (*(argv[2]) == '0') ? FALSE : TRUE;

    /* determine whether Combo should solve the relaxed problem */
    bool_t relx;
    relx = (*(argv[3]) == '0') ? FALSE : TRUE;

    /* determine whether Combo should be executed */
    bool_t exe_combo;
    exe_combo = (*(argv[4]) == '0') ? FALSE : TRUE;

    num_t res = combo_wrap(k, 0, k->capacity, def, relx, exe_combo, 0);

    free_knapsack(k);

    return 0;
}

#ifdef __cplusplus
}
#endif
