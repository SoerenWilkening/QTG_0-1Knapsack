#ifndef KNAPSACK_H
#define KNAPSACK_H

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <gmp.h>
#include <gsl/gsl_rng.h>
#include "syslinks.h"

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
 *                            type definitions
 * =============================================================================
 */

typedef int         bool_t;             /* logical variable         */
typedef int         bit_t;              /* bit/qubit counting       */
typedef long        num_t;              /* large integers           */
typedef double      ratio_t;            /* ratios                   */
typedef uint32_t    count_t;            /* gate, cycle counting     */

/*
 * Struct:          item_t
 * -----------------------
 * Description:     This struct represents an item assignable to a knapsack.
 * Contents:
 *      cost:       Integer cost of the item.
 *      profit:     Integer profit of the item.
 *      included:   Marks whether item is included into knapsack.
 */
typedef struct item {
	num_t profit;
	num_t cost;
    bool_t included;
} item_t;

/*
 * Struct:              knapsack_t
 * -------------------------------
 * Description:         This struct represents a knapsack.
 * Contents:
 *      size:           Integer size of the knapsack.
 *      capacity:       Integer capacity of the knapsack.
 *      remain_cost:    Integer remaining cost of the knapsack.
 *      tot_profit:     Integer total profit of the knapsack.
 *      items:          Array of assigned items.
 *      name:           Instance name.
 */
typedef struct knapsack {
	bit_t size;
	num_t capacity;
	num_t remain_cost;
	num_t tot_profit;
	item_t* items;
    char* name;
} knapsack_t;

/*
 * Struct:              path_t
 * ---------------------------
 * Description:         This struct represents a path with its remaining cost
 *                      and total profit.
 * Contents:
 *      remain_cost:    Remaining cost of the path.
 *      tot_profit:     Total profit of the path.
 *      state:          Bit string representation of the path.
 */
typedef struct path {
    num_t remain_cost;
    num_t tot_profit;
    mpz_t vector;
} path_t;

typedef enum sort {
    COST,
    PROFIT,
    RATIO,
} sort_t;

typedef enum lb {
    IGREEDY,
} lb_t;

typedef enum ub {
    SUM,
    FGREEDY,
} ub_t;

typedef enum category {
    SMALL,
    LARGE,
    HARD,
} category_t;

/* 
 * =============================================================================
 *                            enum names
 * =============================================================================
 */

/*
 * Function:    get_sort_name
 * --------------------------
 * Description: This function returns the name of a sorting mehtod.
 * Parameter:   Sorting method.
 * Returns:     Sorting method's name as a string.
 */
const char* get_sort_name(sort_t);

/*
 * Function:    get_lb_name
 * ------------------------
 * Description: This function returns the name of a lower bound method.
 * Parameter:   Lower bound method.
 * Returns:     Lower bound method's name as a string.
 */
const char* get_lb_name(lb_t);

/*
 * Function:    get_ub_name
 * ------------------------
 * Description: This function returns the name of an upper bound method.
 * Parameter:   Upper bound method.
 * Returns:     Upper bound method's name as a string.
 */
const char* get_ub_name(ub_t);

/*
 * Function:    get_category_name
 * ------------------------------
 * Description: This function returns the name of a category for a Pisinger
 *              knapsack.
 * Parameter:   Category.
 * Returns:     Category's name as a string.
 */
const char* get_category_name(category_t);

/* 
 * =============================================================================
 *                            number of digits
 * =============================================================================
 */

/*
 * Function: 	num_digits
 * -----------------------
 * Description:	This function returns the number of digits of the given input.
 * Parameter:	Number whose amount of digits should be evaluated.
 * Returns:		Number of digits of the given input as an unsigned int.
 */
bit_t num_digits(num_t);

/* 
 * =============================================================================
 *                            create/free
 * =============================================================================
 */

/*
 * Function: 		create_item
 * ----------------------------
 * Description:		This function dynamically allocates a new item holding the
 *                  specified cost and profit.
 * Parameters:
 *		parameter1:	Cost the item should be initialized with.
 *		parameter2:	Profit the item should be initialized with.
 * Returns:			Pointer to the allocated item.
 * Side Effect:		Allocates dynamically; pointer should eventually be freed.
 */
item_t* create_item(num_t, num_t);

/*
 * Function:    free_path
 * ----------------------
 * Description: This function frees a dynamically allocated path together with
 *              its vector.
 * Parameter:   Pointer to the path that should be freed.
 */
void free_path(path_t*);

/*
 * Function: 		create_empty_knapsack
 * --------------------------------------
 * Description:		This function dynamically allocates a new knapsack with
 *                  fixed size, but no assigned item costs or profits.
 * Parameters:
 *		parameter1:	Number of items that are being considered.
 *		parameter2:	Capacity of the knapsack.
 * Returns:			Pointer to the allocated knapsack.
 * Side Effects:
 * 		- Allocates dynamically; pointer should eventually be freed.
 *		- Also allocates items and include statements dynamically; their
 *        pointers should also eventually be freed.
 */
knapsack_t* create_empty_knapsack(bit_t, num_t);

/*
 * Function:        pisinger_filename
 * ----------------------------------
 * Description:     This function formats instance identifier from Pisinger's
 *                  benchmarks to the correct file name.
 * Parameters:
 *      parameter1: Category of knapsack instances.
 *      parameter2: File number within the specified category.
 *      parameter3: Number of items assigned to the knapsack.
 *      parameter4: Range of coefficients.
 *      parameter5: Buffer for filename.
 *      parameter6: Buffer size.
 */
void pisinger_filename(category_t, size_t, bit_t, num_t, char[], size_t);

/*
 * Function:        jooken_filename
 * --------------------------------
 * Description:     This function formats instance identifier from Jooken's
 *                  benchmarks to the correct file name.
 * Parameters:
 *      parameter1: Number of items assigned to the knapsack.
 *      parameter2: Capacity of knapsack.
 *      parameter3: Number of item groups.
 *      parameter4: Ratio of items in the last group.
 *      parameter5: Pertubation for costs and profits.
 *      parameter6: Value range.
 *      parameter7: Buffer for the filename.
 *      parameter8: Buffer size.
 */
void jooken_filename(bit_t, num_t, bit_t, double, double, num_t, char[], \
                     size_t);

/*
 * Function:        create_pisinger_knapsack
 * -----------------------------------------
 * Description:     This function dynamically allocates a new knapsack with
 *                  parameters specified in the given file. The format is
 *                  adapted to Pisinger's benchmarks for the 0-1-Knapsack
 *                  problem.
 * Parameter:       File name. 
 * Returns:         Pointer to the allocated knapsack.
 * Side Effects:   
 *      - Allocates dynamically; pointer should eventually be freed.
 *      - Also allocates items and include statements dynamically; their
 *        pointers should also eventually be freed.
 */
knapsack_t* create_pisinger_knapsack(char*);

/*
 * Function:        create_jooken_knapsack
 * ---------------------------------------
 * Description:     This function dynamically allocates a new knapsack with
 *                  parameters specified in the given file. The format is
 *                  adapted to Jooken's benchmarks for the 0-1-Knapsack
 *                  problem.
 * Parameter:       File name.
 * Returns:         Pointer to the allocated knapsack.
 * Side Effects:
 *      - Allocates dynamically; pointer should eventually be freed.
 *      - Also allocates items and include statements dynamically; their
 *        pointers should also eventually be freed.
 */
knapsack_t* create_jooken_knapsack(char*);

/*
 * Function:    copy_knapsack
 * --------------------------
 * Description: This function dynamically allocates a deep copy of a given
 *              knapsack instance.
 * Parameters:  Pointer to the knapsack that should be copied.
 * Returns:     Pointer to the allocated knapsack.
 * Side Effects:
 *      - Allocates dynamically; pointer should eventually be freed.
 *      - Also allocates items and include statements dynamically; their
 *        pointers should also eventually be freed.
 */
knapsack_t* copy_knapsack(const knapsack_t*);

/*
 * Function: 		assign_item_values
 * -----------------------------------
 * Description:		This function assigns costs and profits to knapsack items.
 * Parameters:
 *		parameter1:	Pointer to the knapsack holding the items.
 *		parameter2:	Array of costs that should be assigned to the items.
 *		parameter3: Array of profits that should be assigned to the items.
 */
void assign_item_values(knapsack_t*, num_t[], num_t[]);

/*
 * Function:	free_knapsack
 * --------------------------
 * Description:	This function frees a dynamically allocated knapsack together
 *              with its item array.
 * Parameter:	Pointer to the knapsack that should be freed.
 */
void free_knapsack(knapsack_t*);

/* 
 * =============================================================================
 *                            item manipulation
 * =============================================================================
 */

 /*
 * Function:        put_item
 * -------------------------
 * Description:     This function puts an item into a knapsack, lowering the
 *                  remaining cost and increasing the total profit. If including
 *                  the item was successful, one (true) is returned. If the item
 *                  is already included or the remaining cost does not allow to
 *                  include it, nothing happens and zero (false) is returned
 *                  instead.
 * Parameters:
 *      parameter1: Pointer to knapsack in which the item should be included.
 *      parameter2: Index of item that should be included into knapsack.
 * Returns:         Whether including the item was successful or not.
 */
bool_t put_item(knapsack_t*, bit_t);

/*
 * Function:        remove_item
 * ----------------------------
 * Description:     This function removes an item from a knapsack, increasing
 *                  the remaining cost and lowering the total profit. If
 *                  removing the item was successful, one (true) is returned. If
 *                  the item is already excluded, nothing happens and zero
 *                  (false) is returned instead.
 * Parameters:
 *      parameter1: Pointer to knapsack from which the item should be removed.
 *      parameter2: Index of item that should be excluded from knapsack.
 * Returns:         Whether removing the item was successful or not.
 */
bool_t remove_item(knapsack_t*, bit_t);

/*
 * Function:    remove_all_items
 * -----------------------------
 * Description: This function removes all items from a knapsack, resetting
 *              remaining cost and total profit.
 * Parameters:  Pointer to knapsack whose items should be removed.
 */
void remove_all_items(knapsack_t*);

/*
 * Function:        swap_knapsack_items
 * ------------------------------------
 * Description:     This function swaps the costs and the profits of two items
 *                  in a knapsack.
 * Parameters:
 *      parameter1: Index of first item in knapsack.
 *      parameter2: Index of second item in knapsack.
 */
void swap_knapsack_items(knapsack_t*, bit_t, bit_t);

/*
 * Function:        knapsack_partition
 * -----------------------------------
 * Description:     This function partitions the items of a knapsack between two
 *                  indices into two sets separated by a partition index which
 *                  is returned. The partition is specified by the given sorting
 *                  method. The item at the upper index is used as pivot; all
 *                  items between the lower and the upper index are compared.
 * Parameters:
 *      parameter1: Pointer to knapsack whose items should be partitioned.
 *      parameter2: Lower index of items that should be considered.
 *      parameter3: Upper index of items that should be considered; also marks
 *                  pivot element.
 *      parameter4: Sorting method that should be used for the partition.
 * Returns:         Index of pivot element after sorting.
 */
int32_t knapsack_partition(knapsack_t*, int32_t, int32_t, sort_t);

/*
 * Function:        knapsack_quicksort
 * -----------------------------------
 * Description:     This function sorts the items of a knapsack between two
 *                  indices using quicksort. The given sorting method specifies
 *                  how the items are sorted. The quicksort routine calls the
 *                  partition routine recursively, thus also using the item at
 *                  the upper index as pivot.
 * Parameters:
 *      parameter1: Pointer to knapsack whose items should be sorted.
 *      parameter2: Lower index of items that should be sorted.
 *      parameter3: Upper index of items that should be sorted; also marks the
 *                  initial pivot element.
 *      parameter4: Method that should be used for the sorting.
 */
void knapsack_quicksort(knapsack_t*, int32_t, int32_t, sort_t);

/*
 * Function:        sort_knapsack
 * ------------------------------
 * Description:     This function sorts all the knapsack items using quicksort.
 *                  The sorting method specifies how the items are sorted. The
 *                  sorting routine calls quicksort with lower index 0 and upper
 *                  index set to the knapsack's size - 1.
 * Parameters:
 *      parameter1: Pointer to knapsack whose items should be sorted.
 *      parameter2: Method that should be used for the sorting.
 */
void sort_knapsack(knapsack_t*, sort_t);

/*
 * Function:    apply_int_greedy
 * -----------------------------
 * Description: This function includes items to a knapsack, following the
 *              integer greedy method. Items are included as long as the
 *              remaining cost stays non-negative. This method does not sort the
 *              knapsack items in advance.
 * Parameter:   Pointer to knapsack whose items should be included.
 */
void apply_int_greedy(knapsack_t*);

/* 
 * =============================================================================
 *                            knapsack information
 * =============================================================================
 */

/*
 * Function:    bit_rep
 * --------------------
 * Description:     This function writes the current assignment of items of a
 *                  knapsack in a given bit string.
 * Parameters:
 *      parameter1: Pointer to knapsack for which the current item assignment
 *                  should be translated into a bit string.
 *      parameter2: Bit string which should carry the current item assignment.
 */
void bit_rep(const knapsack_t*, mpz_t);

/*
 * Function:    path_rep
 * ---------------------
 * Description: This function writes the current assignment of items of a
 *              knapsack in a path and returns a pointer to it.
 * Parameter:   Pointer to knapsack which should be represented as a path.
 * Returns:     Pointer to path representing the knapsack.
 * Side Effect: Allocates dynamically; pointer should eventually be freed.
 */
path_t* path_rep(const knapsack_t*);

/*
 * Function:    tot_cost
 * ---------------------
 * Description: This function returns the total cost of all knapsack items.
 * Parameter:   Pointer to knapsack for which the total item cost should be
 *              calculated.
 * Returns:     Total cost of all knapsack items.
 */
num_t tot_cost(const knapsack_t*);

/*
 * Function:    max_cost
 * ---------------------
 * Description: This function returns the maximum cost of all knapsack items.
 * Parameter:   Pointer to knapsack for which the maximum item cost should be
 *              calculated.
 * Returns:     Maximum item cost among the knapsack.
 */
num_t max_cost(const knapsack_t*);

/*
 * Function:    min_cost
 * ---------------------
 * Description: This function returns the minimum cost of all knapsack items.
 * Parameter:   Pointer to knapsack for which the minimum item cost should be
 *              calculated.
 * Returns:     Minimum item cost among the knapsack.
 */
num_t min_cost(const knapsack_t*);

/*
 * Function:    max_profit
 * -----------------------
 * Description: This function returns the maximum profit of all knapsack items.
 * Parameter:   Pointer to knapsack for which the maximum item profit should be
 *              calculated.
 * Returns:     Maximum item profit among the knapsack.
 */
num_t max_profit(const knapsack_t*);

/*
 * Function:    min_profit
 * -----------------------
 * Description: This function returns the minimum profit of all knapsack items.
 * Parameter:   Pointer to knapsack for which the minimum item profit should be
 *              calculated.
 * Returns:     Minimum item profit among the knapsack.
 */
num_t min_profit(const knapsack_t*);

/*
 * Function:    max_ratio
 * ----------------------
 * Description: This function returns the maximum ratio of profit and cost of
 *              all knapsack items.
 * Parameter:   Pointer to knapsack for which the maximum item ratio should be
 *              calculated.
 * Returns:     Maximum item ratio among the knapsack.
 */
ratio_t max_ratio(const knapsack_t*);

/*
 * Function:    min_ratio
 * ----------------------
 * Description: This function returns the minimum ratio of profit and cost of
 *              all knapsack items.
 * Parameter:   Pointer to knapsack for which the minimum item ratio should be
 *              calculated.
 * Returns:     Minimum item ratio among the knapsack.
 */
ratio_t min_ratio(const knapsack_t*);

/*
 * Function:    profit_sum
 * -----------------------
 * Description: This function calculates the sum of all knapsack items' profit.
 * Parameter:   Pointer to knapsack whose cummulative items' profit should be
 *              calculated.
 * Returns:     Sum of all items' profit.
 */
num_t profit_sum(const knapsack_t*);

/*
 * Function:    cost_sum
 * ---------------------
 * Description: This function calculates the sum of all knapsack items' cost.
 * Parameter:   Pointer to knapsack whose cummulative items' cost should be
 *              calculated.
 * Returns:     Sum of all items' cost.
 */
num_t cost_sum(const knapsack_t*);

/*
 * Function:        is_trivial
 * ---------------------------
 * Description:     This function checks whether given knapsack instance is
 *                  trivial and if so saves the optimal profit at the given
 *                  address.
 * Parameters:   
 *      parameter1: Pointer to knapsack which should be checked.
 *      parameter2: Pointer to the position where the optimal profit should be
 *                  saved.
 * Returns:         Whether knapsack is trivial or not.
 */
bool_t is_trivial(const knapsack_t*, num_t*);

/*
 * Function:    break_item
 * -----------------------
 * Description: This function checks returns the index of a given knapsack
 *              instance's break item.
 * Parameter:   Pointer to knapsack whose break item should be inferred.
 * Returns:     The break item's index.
 */
bit_t break_item(const knapsack_t* k);

/*
 * Function:    int_greedy
 * -----------------------
 * Description:     This function calculates the total profit that arises when
 *                  including items in a greedy manner. Items, in a specified
 *                  sorting, are included as long as the remaining cost stays
 *                  non-negative. The knapsack is copied to the stack and the
 *                  items are included there, leaving the original unchanged.
 * Parameters:      
 *      parameter1: Pointer to knapsack whose integer greedy value should be
 *                  calculated.
 *      parameter2: Sorting method for the knapsack before applying integer
 *                  greedy.
 * Returns:         Total profit achieved by the integer greedy method.
 * Side Effect:     Temporarily creates a knapsack copy on the stack.
 */
num_t int_greedy(const knapsack_t*, sort_t);

/*
 * Function:    frac_greedy
 * ------------------------
 * Description:     This function calculates the total profit that could be
 *                  achieved by putting fractions of items into a knapsack.
 *                  Since this would generally yield to an infeasible solutions,
 *                  the items are not really included. Instead the knapsack is
 *                  copied on the stack.
 * Parameter:   
 *      parameter1: Pointer to knapsack whose fractional greedy value should be
 *                  calculated.
 *      parameter2: Sorting method for the knapsack before applying fractional
 *                  greedy.
 * Returns:         Total profit achieved by the fractional greedy method.
 * Side Effect:     Temporarily creates a knapsack copy on the stack.
 */
num_t frac_greedy(const knapsack_t*, sort_t);

/*
 * Function:        get_lb
 * -----------------------
 * Description:     This function calculates a lower bound to the total profit
 *                  of a knapsack.
 * Parameters:
 *      parameter1: Pointer to knapsack for which a lower bound to the total
 *                  profit should be calculated.
 *      parameter2: Method for determining the lower bound.
 * Returns:         Lower bound to the total profit.
 * Side Effect:     May temprarily create a knapsack copy on the stack.
 */
num_t get_lb(const knapsack_t*, lb_t);

/*
 * Function:        get_ub
 * -----------------------
 * Description:     This function calculates an upper bound to the total profit
 *                  of a knapsack.
 * Parameters:
 *      parameter1: Pointer to knapsack for which an upper bound to the total
 *                  profit should be calculated.
 *      parameter2: Method for determining the upper bound.
 * Returns:         Upper bound to the total profit.
 * Side Effect:     May temporarily create a knapsack copy on the stack.
 */
num_t get_ub(const knapsack_t*, ub_t);

/*
 * Function:    print_knapsack
 * ---------------------------
 * Description: This function prints the current state of a knapsack.
 *              Information about the knapsack's size, capicity, remaining cost,
 *              and total profit are provided. A table of all items together
 *              with costs, profits, and whether they are included or not is
 *              shown.
 * Parameter:   Pointer to knapsack whose state should be printed.
 */
void print_knapsack(const knapsack_t*);

#ifdef __cplusplus
}
#endif

#endif
