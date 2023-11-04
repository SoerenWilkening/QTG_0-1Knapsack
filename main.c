/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include "include\simulate.h"
#else
#include "include/simulate.h"
#endif


#include "include/expknapwrp.h"
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
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

/* 
 * =============================================================================
 *                            declare global RNG
 * =============================================================================
 */

gsl_rng *r;  /* global generator */

/* 
 * =============================================================================
 *                            main
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    const gsl_rng_type *T;
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    time_t rawtime;
    time(&rawtime);
    gsl_rng_set(r, rawtime);

    size_t bias, max_iter, runs_per_instance, i;
    knapsack_t *k;
    path_t *sol;

    /* determine bias */
    bias = atoi(argv[1]);

    /* determine maximum number of iteration within Qsearch */
    max_iter = atoi(argv[2]);

    /* determine runs per instance */
    runs_per_instance = atoi(argv[3]);

    FILE *file_instances = fopen("benchmark_instances.txt", "r");
    char line[256];
    while (fgets(line, sizeof(line), file_instances) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (line[10] == 'p') {
            k = create_jooken_knapsack(line);
        } else {
            k = create_pisinger_knapsack(line);
            line[strlen(line) - 4] = '\0';
            create_dir(line);
        }
        printf("%s\n", k->name);
        for (i = 0; i < runs_per_instance; ++i) {
//            long zzz = expkap_wrap(k, 0, k->capacity, 1, 0, 1, 1);
//            printf("sol = %ld\n", zzz);
//            measure_combo(k);
            sol = q_max_search(k, bias, COMPARE, max_iter, r);
            free_path(sol);
            printf("\33[2K\r%4zu-th iteration done.", i + 1);
            fflush(stdout);
        }
        printf("\n");
        free_knapsack(k);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
