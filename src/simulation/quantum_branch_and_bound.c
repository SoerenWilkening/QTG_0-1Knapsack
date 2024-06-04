#include "common/knapsack.h"
#include "combo/combowrp.h"

typedef unsigned char qbnb_method;
#define Ambainis 0
#define Montanaro 1


long branch_and_bound(knapsack_t *k, int level, long threshold, size_t *count_nodes, long current_objective,
                      long remaining_capacity, int *exec, qbnb_method method);

num_t bnb_greedy(const knapsack_t *k, long cap, int first_item);


int main() {
    printf("We will now benchmark quantum branch and bound!\n");

    // change this to "Montanaro" to execute Montanaros Branch-and-Bound
    qbnb_method method = Ambainis;

    knapsack_t *k;
    char file[256], type[10];
    size_t bias, max_iter, runs_per_instance, i;

    FILE *file_instances = fopen("../benchmark_instances.txt", "r");
    char line[256];
    while (fgets(line, sizeof(line), file_instances) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        sscanf(line, "%s %s %zu %zu %zu", type, file, &bias, &max_iter, &runs_per_instance);

        if (strcmp(type, "KP") == 0) {
            k = create_jooken_knapsack(file);
        } else {
            printf("Wrong Problem!");
            return 0;
        }
        long exact = combo_wrap(k, 0, k->capacity, 0, 0, 1, 0);

        int exec = 1;
        long bnb;
        size_t count_nodes = 0;
        if (method == Montanaro) count_nodes = 1e10;
        else bnb = branch_and_bound(k, 0, exact, &count_nodes, 0L, k->capacity, &exec, method);
        printf("%d %ld %ld %ld\n", k->size, exact, bnb, count_nodes);

        /* The walk operator contains two diffusion operators */
        count_t cycle_count = 2 * cycleCountQbnbDiffucsionOperator(k);
        uint64_t phase_estimation_oracles = (1. + 1. / pow(.1, 1.5)) * pow(2, ceil(log2(sqrt(count_nodes * k->size))) + 1);

        char bnb_name[512];

        sprintf(bnb_name, "mkdir -p %s/qbnb", k->name);
        system(bnb_name);
        if (method == Ambainis) sprintf(bnb_name, "%s/qbnb/Ambainis.txt", k->name);
        else if (method == Montanaro) sprintf(bnb_name, "%s/qbnb/Montanaro.txt", k->name);
        uint64_t total_count = phase_estimation_oracles * cycle_count;

        FILE *save_qbnb = fopen(bnb_name, "a");
        fprintf(save_qbnb, "%llu\n", total_count);
        fclose(save_qbnb);

        free_knapsack(k);
    }
}



num_t bnb_greedy(const knapsack_t *k, long cap, int first_item) {
    num_t tot_profit = 0, remaining_cap = cap;

    int j;
    for (j = first_item; j < k->size; ++j) {
        if (k->items[j].weight <= remaining_cap) {
            tot_profit += k->items[j].profit;
            remaining_cap -= k->items[j].weight;
        } else { break; }
    }
    if (j == k->size) return  tot_profit;

    double frac_item = (double) k->items[j].profit / k->items[j].weight * remaining_cap;
    return tot_profit + ((num_t) floor(frac_item));
}

long branch_and_bound(knapsack_t *k, int level, long threshold, size_t *count_nodes, long current_objective,
                      long remaining_capacity, int *exec, qbnb_method method) {

    if (current_objective == threshold && method == Ambainis) *exec = 0;
    if (level == k->size || exec == 0 || *count_nodes >= 1e10) return current_objective;

    long left_ub, right_ub;
    left_ub = current_objective + bnb_greedy(k, remaining_capacity, level);
    right_ub = current_objective + value(k->profit, level, k->vT) +
               bnb_greedy(k, remaining_capacity - k->items[level].weight, level + 1);

    long right_val = 0, left_val = 0;
    if (((remaining_capacity - k->items[level].weight) >= 0) && right_ub >= threshold  && *exec == 1) {
        (*count_nodes)++;
        right_val = branch_and_bound(k, level + 1, threshold, count_nodes,
                                     current_objective + k->items[level].profit,
                                     remaining_capacity - k->items[level].weight, exec, method);
    }
    if (left_ub >= threshold && *exec == 1) {
        (*count_nodes)++;
        left_val = branch_and_bound(k, level + 1, threshold, count_nodes,
                                    current_objective,
                                    remaining_capacity, exec, method);
    }
    if (right_val >= threshold) return right_val;
    else if (left_val >= threshold) return left_val;
}