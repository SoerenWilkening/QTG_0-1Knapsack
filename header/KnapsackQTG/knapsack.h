#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>

typedef uint32_t bit_t;             /* bit/qubit counting   */
typedef uint64_t num_t;             /* large integers       */
typedef double ratio_t;             /* ratios               */
typedef uint32_t count_t;           /* gate, cycle counting */

//bit_t length_of_num_t;

/*
 * Struct:      item
 * -----------------
 * Description: This struct represents an item that can be assigned to a knapsack.
 * Contents:
 *      cost:   Integer cost of the item (stored as an unsigned long).
 *      profit: Integer profit of the item (stored as an unsigned long).
 */
typedef struct item_t{
	num_t cost;
	num_t profit;
} item_t;

/*
 * Struct:      knapsack
 * ---------------------
 * Description: This struct represents an item that can be assigned to a knapsack.
 * Contents:
 *      size:           Integer size of the knapsack; 
 *      capacity:       Integer capacity of the knapsack; determines the initial remaining cost.
 *      remainingCost:  Integer remaining cost of the knapsack; initially set to the capacity.
 *      totalProfit:    Integer total profit of the knapsack; given by the sum of included item's profits.
 *      items:          Pointer to the items assigned to the knapsack.
 *      included:       Pointer to booleans marking whether items are included in the knapsack or not.
 */
typedef struct knapsack{
	bit_t size;
	num_t capacity;
	num_t remain_cost;
	num_t tot_profit;
	item_t* items;
	uint8_t* included;
} knapsack_t;

typedef enum sort {
    COST,
    PROFIT,
    RATIO
} sort_t;

typedef enum lb {
    IGREEDY,
} lb_t;

typedef enum ub {
    SUM,
    FGREEDY,
} ub_t;

/*
 * Function:    get_sort_name
 * --------------------------
 * Description: This function returns a sorting method's name as a string.
 * Parameter:   Sorting method.
 * Returns:     Sorting method's name as a string.
 */
const char* get_sort_name(sort_t);

/*
 * Function:    get_lb_name
 * ------------------------
 * Description: This function returns the of a lower bound method as a string.
 * Parameter:   Lower bound method.
 * Returns:     Lower bound method's name as a string.
 */
const char* get_lb_name(lb_t);

/*
 * Function:    get_ub_name
 * ------------------------
 * Description: This function returns the of an upper bound method as a string.
 * Parameter:   Upper bound method.
 * Returns:     Upper bound method's name as a string.
 */
const char* get_ub_name(ub_t);

/*
 * Function: 	num_digits
 * -----------------------
 * Description:	This function returns the number of digits of the given input.
 * Parameter:	Number whose amount of digits should be evaluated (handed as an unsigned long).
 * Returns:		Number of digits of the given input as an unsigned int.
 */
bit_t num_digits(num_t);

/*
 * Function: 		create_item
 * ----------------------------
 * Description:		This function dynamically allocates a new item holding the sepcified cost and profit.
 * Parameters:
 *		parameter1:	Cost the item should be initialized with.
 *		parameter2:	Profit the item should be initialized with.
 * Returns:			Pointer to the allocated item.
 * Side Effect:		Allocates dynamically; thus the pointer should eventually be freed.
 */
item_t* create_item(num_t, num_t);

/*
 * Function: 		create_empty_knapsack
 * --------------------------------------
 * Description:		This function dynamically allocates a new knapsack with fixed size, but no assigned item costs or profits.
 * Parameters:
 *		parameter1:	Number of items that are being considered; mainly determines the size of the allocated memory.
 *		parameter2:	Capacity of the knapsack.
 * Returns:			Pointer to the allocated knapsack.
 * Side Effects:
 * 		- Allocates dynamically; thus the pointer should eventually be freed.
 *		- Also allocates items and booleans dynamically; there pointers should also eventually be freed.
 */
knapsack_t* create_empty_knapsack(bit_t, num_t);

/*
 * Function: 		assign_item_values
 * -----------------------------------
 * Description:		This function assigns costs and profits to the items within a knapsack.
 * Parameters:
 *		parameter1:	Pointer to the knapsack holding the items.
 *		parameter2:	Array of costs that should be assigned to the items.
 *		parameter3: Array of profits that should be assigned to the items.
 */
void assign_item_values(knapsack_t*, num_t[], num_t[]);

/*
 * Function:	free_knapsack
 * --------------------------
 * Description:	This function frees a dynamically allocated knapsack together with its item array and boolean array.
 * Parameter:	Pointer to the knapsack that should be freed.
 */
void free_knapsack(knapsack_t*);

/*
 * Function:    max_cost
 * ---------------------
 * Description: This function returns the maximum cost among all items assigned to a knapsack.
 * Parameter:   Pointer to knapsack for which the maximum item cost should be calculated.
 * Returns:     Maximum item cost among the knapsack.
 */
num_t max_cost(const knapsack_t*);

/*
 * Function:    min_cost
 * ---------------------
 * Description: This function returns the minimum cost among all items assigned to a knapsack.
 * Parameter:   Pointer to knapsack for which the minimum item cost should be calculated.
 * Returns:     Minimum item cost among the knapsack.
 */
num_t min_cost(const knapsack_t*);

/*
 * Function:    max_profit
 * -----------------------
 * Description: This function returns the maximum profit among all items assigned to a knapsack.
 * Parameter:   Pointer to knapsack for which the maximum item profit should be calculated.
 * Returns:     Maximum item profit among the knapsack.
 */
num_t max_profit(const knapsack_t*);

/*
 * Function:    min_profit
 * -----------------------
 * Description: This function returns the minimum profit among all items assigned to a knapsack.
 * Parameter:   Pointer to knapsack for which the minimum item profit should be calculated.
 * Returns:     Minimum item profit among the knapsack.
 */
num_t min_profit(const knapsack_t*);

/*
 * Function:    max_ratio
 * ----------------------
 * Description: This function returns the maximum ratio of profit and cost among all items assigned to a knapsack.
 * Parameter:   Pointer to knapsack for which the maximum item ratio should be calculated.
 * Returns:     Maximum item ratio among the knapsack.
 */
ratio_t max_ratio(const knapsack_t*);

/*
 * Function:    min_ratio
 * ----------------------
 * Description: This function returns the minimum ratio of profit and cost among all items assigned to a knapsack.
 * Parameter:   Pointer to knapsack for which the minimum item ratio should be calculated.
 * Returns:     Minimum item ratio among the knapsack.
 */
ratio_t min_ratio(const knapsack_t*);

/*
 * Function: 		put_item
 * -------------------------
 * Description:		This function puts an item into a knapsack, lowering the remaining cost and increasing the total profit.
 *					If including the item was successful, true is returned.
 *					If the item is already included or the remaining cost does not allow to include it, nothing happens and false is returned.
 * Parameters:
 *		parameter1:	Pointer to the knapsack in which the item should be included.
 *		parameter2:	Index of the item that should be included into the knapsack.
 * Returns:			Whether including the item was successful or not.
 */
uint8_t put_item(knapsack_t*, bit_t);

/*
 * Function: 		remove_item
 * ----------------------------
 * Description:		This function removes an item from a knapsack, increasing the remaining cost and lowering the total profit.
 *					If removing the item was successful, true is returned.
 *					If the item is already excluded, nothing happens and false is returned.
 * Parameters:
 *		parameter1:	Pointer to the knapsack from which the item should be removed.
 *		parameter2:	Index of the item that should be excluded from the knapsack.
 * Returns:			Whether removing the item was successful or not.
 */
uint8_t remove_item(knapsack_t*, bit_t);

/*
 * Function: 	remove_all_items
 * -----------------------------
 * Description:	This function removes all items from a knapsack, resetting remaining cost and total profit.
 * Parameters:	Pointer to the knapsack whose items should be removed.
 */
void remove_all_items(knapsack_t*);

/*
 * Function: 		swap_knapsack_items
 * ------------------------------------
 * Description:		This function swaps the costs and the profits of two items in a knapsack.
 * Parameters:
 *		parameter1:	Index of the first item in the knapsack.
 *		parameter2:	Index of the second item in the knapsack.
 */
void swap_knapsack_items(knapsack_t*, bit_t, bit_t);

/*
 * Function: 		knapsack_partition
 * ----------------------------------
 * Description:		This function partitions the items of a knapsack between two indices into two sets separated by a partition index which is returned.
 *					The partition is specified by the given Sorting (profit, cost or ratio).
 *					The item at the upper index is used as pivot; all items between the lower and the upper index are compared.
 * Parameters:
 *		parameter1:	Pointer to the knapsack whose items should be partitioned.
 *		parameter2:	Lower index of items that should be considered.
 *		parameter3: Upper index of items that should be considered; also marks pivot element.
 *		parameter4: Sorting method that should be used for the partition (profit, cost or ratio).
 * Returns:			Index of the pivot element after sorting.
 */
int32_t knapsack_partition(knapsack_t*, int32_t, int32_t, sort_t);

/*
 * Function: 		knapsack_quicksort
 * -----------------------------------
 * Description:		This function sorts the items of a knapsack between two indices using quicksort.
 *					The given Sorting specifies how the items are sorted (profit, cost or ratio).
 *					The quicksort routine calls the partition recursively, thus also using the item at the upper index as pivot.
 * Parameters:
 *		parameter1:	Pointer to the knapsack whose items should be sorted.
 *		parameter2:	Lower index of items that should be sorted.
 *		parameter3: Upper index of items that should be sorted; also marks the initial pivot element.
 *		parameter4: Method that should be used for the sorting (profit, cost or ratio).
 */
void knapsack_quicksort(knapsack_t*, int32_t, int32_t, sort_t);

/*
 * Function: 		sort_knapsack
 * ------------------------------
 * Description:		This function sorts all the items of a knapsack using quicksort.
 *					The given Sorting specifies how the items are sorted (profit, cost or ratio).
 *					The sorting routine calls quicksort with lower index 0 and upper index set to the knapsack's size.
 * Parameters:
 *		parameter1:	Pointer to the knapsack whose items should be sorted.
 *		parameter2: Method that should be used for the sorting (profit, cost or ratio).
 */
void sort_knapsack(knapsack_t*, sort_t);

/*
 * Function: 	print_knapsack
 * ---------------------------
 * Description:	This function prints the current state of a knapsack.
 *				Information about the knapsack's size, capicity, remaining cost, and total profit are provided.
 *				A table of all items together with costs, profits, and whether they are included or not shown.
 * Parameter:	Pointer to the knapsack whose state should be printed.
 */
void print_knapsack(knapsack_t*);

/*
 * Function:    apply_int_greedy
 * -----------------------------
 * Description:	This function includes items to a knapsack, following the integer greedy method.
 *				Items are included as long as the remaining cost stays non-negative. 
 * Parameter:	Pointer to the knapsack whose items should be included.
 */
void apply_int_greedy(knapsack_t*);

/*
 * Function:    int_greedy
 * -----------------------
 * Description: This function calculates the total profit that arises when including items in a greefy manner.
 *              Items, sorted by their profits, ar included as long as the remaining cost stays non-negative.
 *              The knapsack is copied to the stack and the items are included their, leving the original unchanged.
 * Parameter:   Pointer to the knapsack whose integer greedy value should be calculated.
 * Returns:     Total profit achieved by the integer greedy method.
 * Side Effect: Temprarily creates a knapsack copy on the stack.
 */
num_t int_greedy(const knapsack_t*);

/*
 * Function:        get_lb
 * -----------------------
 * Description:     This function calculates a lower bound to the total profit of a knapsack.
 * Parameters:
 *      parameter1: Pointer to the knapsack for which a lower bound on the total profit should be calculated.
 *      parameter2: Method how the lower bound should be determined.
 * Returns:         Lower bound for the total profit.
 * Side Effect:     May temprarily create a knapsack copy on the stack.
 */
num_t get_lb(const knapsack_t*, lb_t);

/*
 * Function:    profit_sum
 * -----------------------
 * Description: This function calculates the sum of all items assigned to a knapsack.
 * Parameter:   Pointer to the knapsack whose cummulative items' profit should be calculated.
 * Returns:     Sum of all items' profits.
 */
num_t profit_sum(const knapsack_t*);

/*
 * Function:    frac_greedy
 * ------------------------
 * Description: This function calculates the total profit that could be achieved by putting fractions of items into a knapsack.
 *              Since this would generally yield to an infeasible solutions, the items are not really included.
 *              Instead the knapsack is copied on the stack.
 * Parameter:   Pointer to the knapsack whose fractional greedy value should be calculated.
 * Returns:     Total profit achieved by the fractional greedy method.
 * Side Effect: Temprarily creates a knapsack copy on the stack.
 */
num_t frac_greedy(const knapsack_t*);

/*
 * Function:        get_ub
 * -----------------------
 * Description:     This function calculates an upper bound to the total profit of a knapsack.
 * Parameters:
 *      parameter1: Pointer to the knapsack for which an upper bound on the total profit should be calculated.
 *      parameter2: Method how the upper bound should be determined.
 * Returns:         Upper bound for the total profit.
 * Side Effect:     May temprarily create a knapsack copy on the stack.
 */
num_t get_ub(const knapsack_t*, ub_t);

#endif