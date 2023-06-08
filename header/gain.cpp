#include "combo_wrap.h"

#define TRUE                    1
#define FALSE                   0

int main() {
	knapsack_t* k = create_pisinger_knapsack(SMALL, 1, 50, 1000, 1);

	print_knapsack(k);

	sort_knapsack(k, RATIO);

	apply_int_greedy(k);

	print_knapsack(k);

	remove_all_items(k);

	print_knapsack(k);

	num_t combo_res = cpp_combo_wrap(k, 0, FALSE, FALSE, TRUE);

	free_knapsack(k);

	return 0;
}