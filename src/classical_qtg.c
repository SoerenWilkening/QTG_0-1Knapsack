#include "knapsack.h"
#include "combowrp.h"
#include <time.h>
#include <gsl/gsl_rng.h>

#define TRUE                    1
#define FALSE                   0

gsl_rng* r;  /* global generator */

int main() {

	double bias = 0.5;

	const gsl_rng_type* T;

	gsl_rng_env_setup();

	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	double random_num;

	int i,j;

	knapsack_t* k = create_pisinger_knapsack(SMALL, 1, 50, 1000, 1);

	sort_knapsack(k, RATIO);

	apply_int_greedy(k);

	print_knapsack(k);

	num_t cur_opt = k->tot_profit;
	printf("Starting objective value is: %"PRIu64"\n", (uint64_t)cur_opt);

	mpz_t cur_sol;
	mpz_init2(cur_sol, k->size);
	bit_rep(k, cur_sol);

	remove_all_items(k);

	size_t iteration = 0;

	num_t exact_opt = combo_wrap(k, 0, k->capacity, TRUE, FALSE, TRUE, NULL);

	printf("The optimal profit is given by %"PRIu64".\n", (uint64_t)exact_opt);

	while (iteration < 10000000) {
		for (bit_t i = 0; i < k->size; ++i) {
			if (k->items[i].cost <= k->remain_cost) {
				random_num = gsl_rng_uniform(r);
				if (random_num > (1 + (1 - mpz_tstbit(cur_sol, k->size - i - 1)) \
                          * bias) / (bias + 2)) {
					put_item(k, i);
				}
			}
		}
		if (k->tot_profit > cur_opt) {
			cur_opt = k->tot_profit;
			printf("Updated optimum: %"PRIu64".\n", (uint64_t)cur_opt);
			for (bit_t j = 0; j < k->size; ++j) {
				bit_rep(k, cur_sol);
			}
			if (cur_opt == exact_opt) {
				printf("Achieved optimum of %"PRIu64" after %zu iterations.\n", \
					   (uint64_t)cur_opt, iteration);
				break;
			}
		}
		remove_all_items(k);
		++iteration;
	}

	free_knapsack(k);
	gsl_rng_free(r);

	return 0;
}