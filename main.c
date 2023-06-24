#include "header/simulate.h"

#define TRUE                    1
#define FALSE                   0

gsl_rng* r;  /* global generator */

int main(int argc, char* argv[]) {
	const gsl_rng_type* T;
	gsl_rng_env_setup();

	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	size_t bias, max_iter, runs_per_instance, i;
	knapsack_t* k;
	path_t* sol;

	/* determine bias */
	bias = atoi(argv[1]);

	/* determine maximum number of iteration within Qsearch */
	max_iter = atoi(argv[2]);

	/* determine runs per instance */
	runs_per_instance = atoi(argv[3]);

	FILE* file_instances = fopen("benchmark_instances.txt", "r");
	char line[256];
	while (fgets(line, sizeof(line), file_instances) != NULL) {
		line[strcspn(line, "\n")] = '\0';
		k = create_jooken_knapsack(line);
		// printf("test: %"PRIu64"\n", (uint64_t)combo_wrap(k, 0, k->capacity, 0, 0, 1));
		for (i = 0; i < runs_per_instance; ++i) {
			measure_combo(k);
			sol = q_max_search(k, bias, COMPARE, max_iter, r);
	 		free_path(sol);
	 		printf("%zu-th iteration done.\n", i + 1);
		}
		free_knapsack(k);
	}

	return 0;
}