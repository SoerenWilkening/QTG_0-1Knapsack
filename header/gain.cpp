#include "combo_wrap.h"

#define TRUE                    1
#define FALSE                   0

int main() {
	knapsack_t* k = create_pisinger_knapsack(SMALL, 1, 50, 1000, 1);


	sort_knapsack(k, RATIO);

	apply_int_greedy(k);

	print_knapsack(k);

	num_t profit_sum = 0;
	num_t cost_sum = 0;
	for(bit_t i = 0; i < 29; ++i) {
		profit_sum += k->items[i].profit;
		cost_sum += k->items[i].cost;
	} 

	remove_all_items(k);

	num_t combo_res = cpp_combo_wrap(k, 0, TRUE, TRUE);

	printf("Int greedy got: %ld, Frac greedy got: %ld, combo got: %ld\n", profit_sum, frac_greedy(k, RATIO), combo_res);

	free_knapsack(k);

	return 0;
}