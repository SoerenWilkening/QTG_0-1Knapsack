#include "knapsack.h"
#include <stdlib.h>
#include <time.h>
#include "dcmt-master/include/dc.h"

#define TRUE                    1
#define FALSE                   0

int main() {

	int i,j;
    mt_struct *mts;

    /* This trys to find a small Mersenne Twister with period 2^521-1. */
    mts = get_mt_parameter_st(32,521,4172);
    if (mts == NULL) {
        printf ("error\n");
    }
    else {
        sgenrand_mt(3241, mts);
        for (i=0; i<100; i++) {
            for (j=0; j<5; j++)
                printf("%8"PRIx32" ", genrand_mt(mts));
            printf("\n");
        }
        free_mt_struct(mts);
    }

	double random_num;

	printf("Hi\n");

	knapsack_t* k = create_pisinger_knapsack(SMALL, 1, 50, 1000, 1);

	bool_t included[k->size];

	sort_knapsack(k, RATIO);

	apply_int_greedy(k);

	print_knapsack(k);

	for (bit_t i = 0; i < k->size; ++i) {
		included[i] = k->items[i].included;
	}

	num_t cur_opt = k->tot_profit;
	printf("Starting objective value is: %"PRIu64"\n", (uint64_t)cur_opt);

	remove_all_items(k);

	size_t iteration = 0;

	while (iteration < 10) {
		for (bit_t i = 0; i < k->size; ++i) {
			if (k->items[i].cost <= k->remain_cost) {
				srand(time(NULL));
				random_num = (double)rand() / RAND_MAX; // Generate a random number between 0 and 1
				printf("%f", random_num);
				if (random_num < 1. / 2) {
					put_item(k, i);
				}
			}
		}
		if (k->tot_profit > cur_opt) {
			cur_opt = k->tot_profit;
			printf("Updated optimum: %"PRIu64".\n", (uint64_t)cur_opt);
			for (bit_t i = 0; i < k->size; ++i) {
				included[i] = k->items[i].included;
			}
		}
		remove_all_items(k);
		++iteration;
	}

	free_knapsack(k);

	return 0;
}