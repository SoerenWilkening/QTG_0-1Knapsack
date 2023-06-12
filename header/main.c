#include "simulate.h"

#define TRUE                    1
#define FALSE                   0

gsl_rng* r;  /* global generator */

int main() {
	double bias = 0.5;
	size_t num_states = 0;
	const gsl_rng_type* T;
	printf("Declaring RNG type worked!\n");

	gsl_rng_env_setup();
	printf("RNG set-up worked!\n");

	T = gsl_rng_default;
	r = gsl_rng_alloc(T);
	printf("Setting RNG worked!\n");

	knapsack_t* k = create_pisinger_knapsack(SMALL, 1, 50, 1000, 1);
	printf("Initializing knapsack worked!\n");

	size_t* rounds;
	*rounds = 0;

	free_knapsack(k);

	return 0;
}