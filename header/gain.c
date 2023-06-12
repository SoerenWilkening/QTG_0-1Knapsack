#include "simulate.h"

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

	knapsack_t* k = create_pisinger_knapsack(SMALL, 1, 50, 1000, 1);

	sort_knapsack(k, RATIO);

	apply_int_greedy(k);

	mpz_t int_sol;
	mpz_init2(int_sol, k->size);

	num_t threshold = k->tot_profit;

	bit_rep(k, int_sol);

	remove_all_items(k);

	print_knapsack(k);

	uint64_t timer = 0;

	num_t exact = combo_wrap(k, 0, k->capacity, &timer, FALSE, FALSE, TRUE, TRUE);

	printf("Integer greedy lb: %ld, exact solution: %ld.\n", threshold, exact);

	node_t* states = qtg(k, threshold, exact, bias, COMPARE, int_sol, &num_states);

	for (size_t i = 0; i < num_states; ++i) {
		gmp_printf("%zu-th state: %Zd ", i + 1, states[i].path.vector);
	 	printf("with total profit %ld\n", states[i].path.tot_profit);
	}

	// size_t rounds = 0;

	// path_t* sol = q_max_search(k, &rounds, bias, COMPARE, 500, r);

	// printf("Optimal solution of %ld found after %zu rounds.\n", sol->tot_profit, rounds);

	// free(sol);

	free_knapsack(k);

	return 0;
}