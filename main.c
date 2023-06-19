#include "header/simulate.h"

#define TRUE                    1
#define FALSE                   0

gsl_rng* r;  /* global generator */

int main() {
	double bias = 0.5;
	size_t num_states = 0;
	const gsl_rng_type* T;

	gsl_rng_env_setup();

	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	// num_t profits[] = {6, 5, 8, 9, 6, 7, 3};
	// num_t costs[] = {2, 3, 6, 7, 5, 9, 4};
	// knapsack_t* k = create_empty_knapsack(7, 9);
	// assign_item_values(k, costs, profits);

	char fname[256];
	//char* fname = "instances/problemInstances/n_400_c_1000000_g_2_f_0.1_eps_0.001_s_100/test.in";

	jooken_filename(1200, 1000000, 2, 0.3, 0.1, 100, fname, sizeof(fname));
	printf("%s\n", fname);
	knapsack_t* k = create_jooken_knapsack(fname);
	// pisinger_filename(SMALL, 1, 50, 1000, fname, sizeof(fname));
	// knapsack_t* k = create_pisinger_knapsack(fname);

	for (size_t i = 0; i < 50; ++i) {
		measure_combo(k);
	}

	// char* executable = "cmbcount";
	// size_t argc = 4;
	// uint64_t mem_count = 0;
	// uint64_t cycle_count = 0;
	// char* argv[4];
	// argv[0] = k->name;
	// argv[1] = "0";
	// argv[2] = "0";
	// argv[3] = "1";

	// rdmd(executable, argc, argv, &mem_count, &cycle_count);

	// printf("Peak memory usage: %"PRIu64"\nElapsed cycles: %"PRIu64"\n", mem_count, cycle_count);

	// knapsack_t* k = create_jooken_knapsack(400, 1000000, 2, 0.1, 0, 100);

	// sort_knapsack(k, RATIO);

	// apply_int_greedy(k);

	// mpz_t int_sol;
	// mpz_init2(int_sol, k->size);

	// num_t threshold = k->tot_profit;

	// bit_rep(k, int_sol);

	// remove_all_items(k);

	// print_knapsack(k);

	// num_t exact = combo_data(k, 0, k->capacity, FALSE, FALSE, TRUE);

	// printf("Integer greedy lb: %ld, exact solution: %ld.\n", threshold, exact);

	// node_t* states = qtg(k, threshold, exact, bias, COMPARE, int_sol, &num_states);

	// for (size_t i = 0; i < num_states; ++i) {
	// 	gmp_printf("%zu-th state: %Zd ", i + 1, states[i].path.vector);
	//  	printf("with total profit %ld\n", states[i].path.tot_profit);
	// }

	// for(size_t i = 0; i < 10; ++i) {
	// 	measure_combo(k);
	// 	sol = q_max_search(k, bias, COMPARE, 100, r);
	// 	free_path(sol);
	// 	printf("%zu-th iteration done.\n", i + 1);
	// 	printf("--------------------------------\n");
	// }

	free_knapsack(k);

	return 0;
}