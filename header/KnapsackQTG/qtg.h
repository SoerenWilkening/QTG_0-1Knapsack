#ifndef QTG_H
#define QTG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "knapsack.h"

typedef enum qft {
    COPPERSMITH,
} qft_t;

typedef enum add {
    DRAPER,
    DIRECT
} add_t;

typedef enum mc {
    TOFFOLI,
} mc_t;

/*
 * Function:    get_qft_name
 * -------------------------
 * Description: This function returns a QFT method's name as a string.
 * Parameter:   Method how the QFT is implemented.
 * Returns:     QFT method's name as a string.
 */
const char *get_qft_name(qft_t);

/*
 * Function:    get_add_name
 * -------------------------
 * Description: This function returns an adder method's name as a string.
 * Parameter:   Method how the adder is implemented.
 * Returns:     Adder method's name as a string.
 */
const char *get_add_name(add_t);

/*
 * Function:    get_mc_name
 * ------------------------
 * Description: This function returns a decomposition method's name for multi-controls as a string.
 * Parameter:   Method how the multi control is decomposed.
 * Returns:     Decomposition method's name as a string.
 */
const char *get_mc_name(mc_t);

/*
 * Function: 	num_bits
 * ---------------------
 * Description:	This function returns the number of bits of the given input.
 * Parameter:	Number whose amount of bits should be evaluated.
 * Returns:		Number of bits of the given input.
 */
bit_t num_bits(num_t);

/*
 * Function:    path_reg_size
 * --------------------------
 * Description: This function calculates the size of the path register used for the QTG on a knapsack instance.
 *              Its size is given by the number of items assigned to the knapsack.
 * Parameters:  Pointer to the knapsack that should be considered.
 * Returns:     Size (number of qubits) of the path register.
 */
bit_t path_reg_size(const knapsack_t *);

/*
 * Function:    cost_reg_size
 * --------------------------
 * Description: This function calculates the size of the cost register used for the QTG on a knapsack instance.
 *              Its size is given by the number of bits necessary to express the capacity.
 * Parameters:  Pointer to the knapsack that should be considered.
 * Returns:     Size (number of qubits) of the cost register.
 */
bit_t cost_reg_size(const knapsack_t *);

/*
 * Function: 		profit_reg_size
 * --------------------------------
 * Description:		This function calculates the size of the profit register used for the QTG on a knapsack instance.
 *					Its size is given by the number of bits necessary to express an upper bound on the total profit.
 * Parameters:
 *		parameter1:	Pointer to the knapsack that should be considered.
 *		parameter2: Method used for obtaining an upper bound on the total profit.
 * Returns:			Size (number of qubits) of the profit register.
 */
bit_t profit_reg_size(const knapsack_t *, ub_t);

/*
 * Function: 		anc_count_qft
 * ------------------------------
 * Description:		This function counts the ancilla qubits of the given QFT implementation on a given register. 
 * Parameters:
 *		parameter1:	Size of the register the QFT is performed on.
 *		parameter2:	Method how the QFT is implemented.
 * Returns:			Number of ancilla qubits of the QFT implementation.
 */
bit_t anc_count_qft(bit_t, qft_t);

/*
 * Function: 		cycle_count_qft
 * --------------------------------
 * Description:		This function counts the cycles of the given QFT implementation on a given register. 
 * Parameters:
 *		parameter1:	Size of the register the QFT is performed on.
 *		parameter2:	Method how the QFT is implemented.
 *      parameter3: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of cycles of the QFT implementation.
 */
count_t cycle_count_qft(bit_t, qft_t, uint8_t);

/*
 * Function: 		gate_count_qft
 * -------------------------------
 * Description:		This function counts the gates of the given QFT implementation on a given register. 
 * Parameters:
 *		parameter1:	Size of the register the QFT is performed on.
 *		parameter2:	Method how the QFT is implemented.
 *      parameter3: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of gates of the QFT implementation.
 */
count_t gate_count_qft(bit_t, qft_t, uint8_t);

/*
 * Function: 		anc_count_add
 * ------------------------------
 * Description:		This function counts the ancilla qubits for an adder implementation on a QFT-ed register. 
 * Parameters:
 *		parameter1:	Size of the QFT-ed register the adder is performed on.
 *		parameter2: Number that should be added to the register's state.
 *		parameter3:	Method how the adder is implemented.
 * Returns:			Number of ancilla qubits of the adder implementation.
 */
bit_t anc_count_add(bit_t, num_t, add_t);

/*
 * Function: 		cycle_count_adder
 * ----------------------------------
 * Description:		This function counts the cycles of an adder implementation on a QFT-ed register. 
 * Parameters:
 *		parameter1:	Size of the QFT-ed register the adder is performed on.
 *		parameter2: Number that should be added to the register's state.
 *		parameter3:	Method how the adder is implemented.
 *      parameter4: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of cycles of the adder implementation.
 */
count_t cycle_count_add(bit_t, num_t, add_t, uint8_t);

/*
 * Function: 		gate_count_add
 * -------------------------------
 * Description:		This function counts the gates of an adder implementation on a QFT-ed register. 
 * Parameters:
 *		parameter1:	Size of the QFT-ed register the adder is performed on.
 *		parameter2: Number that should be added to the register's state.
 *		parameter3:	Method how the adder is implemented.
 *      parameter4: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of gates of the adder implementation.
 */
count_t gate_count_add(bit_t, num_t, add_t, uint8_t);

/*
 * Function: 		anc_count_mc
 * -----------------------------
 * Description:		This function counts the ancilla qubits for decomposing a multi-control single-qubit-gate. 
 * Parameters:
 *		parameter1:	Number of control qubits.
 *		parameter2:	Method how the multi-control gate is decomposed.
 * Returns:			Number of ancilla qubits of the multi-control decomposition.
 */
bit_t anc_count_mc(bit_t, mc_t);

/*
 * Function: 		cycle_count_mc
 * -------------------------------
 * Description:		This function counts the cycles for decomposing a multi-control single-qubit-gate.  
 * Parameters:
 *		parameter1:	Number of control qubits.
 *		parameter2:	Method how the multi-control gate is decomposed.
 *      parameter3: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of cycles of the multi-control decomposition.
 */
count_t cycle_count_mc(bit_t, mc_t, uint8_t);

/*
 * Function: 		gate_count_mc
 * ------------------------------
 * Description:		This function counts the gates for decomposing a multi-control single-qubit-gate. 
 * Parameters:
 *		parameter1:	Number of control qubits.
 *		parameter2:	Method how the multi-control gate is decomposed.
 *      parameter3: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of gates of the multi-control decomposition.
 */
count_t gate_count_mc(bit_t, mc_t, uint8_t);

/*
 * Function:        anc_count_comp
 * -------------------------------
 * Description:     This function counts the ancilla qubits of an integer comparison on a given register. 
 * Parameters:
 *      parameter1: Size of the register the integer comparison is performed on.
 *      parameter2: Integer that is being compared.
 *      parameter3: Method how the multi-control gate is decomposed.
 *      parameter4: Whether the comparison or its negation should be tested.
 * Returns:         Number of ancilla of the integer comparison method.
 */
bit_t anc_count_comp(bit_t, num_t, mc_t, uint8_t);

/*
 * Function:        cycle_count_comp
 * ---------------------------------
 * Description:     This function counts the cycles of an integer comparison on a given register. 
 * Parameters:
 *      parameter1: Size of the register the integer comparison is performed on.
 *      parameter2: Integer that is being compared.
 *      parameter3: Method how the multi-control gate is decomposed.
 *      parameter4: Whether the comparison or its negation should be tested.
 *      parameter5: Whether toffoli gates should be decomposed or not.
 * Returns:         Number of cycles of the integer comparison method.
 */
count_t cycle_count_comp(bit_t, num_t, mc_t, uint8_t, uint8_t);

/*
 * Function:        gate_count_comp
 * --------------------------------
 * Description:     This function counts the gates of an integer comparison on a given register. 
 * Parameters:
 *      parameter1: Size of the register the integer comparison is performed on.
 *      parameter2: Integer that is being compared.
 *      parameter3: Method how the multi-control gate is decomposed.
 *      parameter4: Whether the comparison or its negation should be tested.
 *      parameter5: Whether toffoli gates should be decomposed or not.
 * Returns:         Number of gates of the integer comparison method.
 */
count_t gate_count_comp(bit_t, num_t, mc_t, uint8_t, uint8_t);

/*
 * Function: 		qubit_count_qtg
 * --------------------------------
 * Description:		This function counts the qubits used for an application of the QTG.
 *					The implementation of the QTG's subroutines also have to be specified.
 *					The main registers as well as any ancilla register ccontribute to the total number.
 * Parameters:
 *		parameter1:	Pointer to the knapsack that should be considered.
 *		parameter2:	Method how the upper bound should be determined.
 *		parameter3:	Method how the QFT is implemented.
 *		parameter4: Method how the adder is implemented.
 *		parameter5:	Method how the multi-control gate is decomposed.
 * Returns:			Number of qubits of one application of the QTG.
 */
bit_t qubit_count_qtg(const knapsack_t *, ub_t, qft_t, add_t, mc_t);

/*
 * Function: 		cycle_count_qtg
 * --------------------------------
 * Description:		This function counts the number of cycles used for an application of the QTG.
 *					The implementation of the QTG's subroutines also have to be specified.
 * Parameters:
 *		parameter1:	Pointer to the knapsack that should be considered.
 *		parameter2:	Method how the upper bound should be determined.
 *		parameter3:	Method how the QFT is implemented.
 *		parameter4: Method how the adder is implemented.
 *		parameter5:	Method how the multi-control gate is decomposed.
 *      parameter6: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of cycles of one application of the QTG.
 */
count_t cycle_count_qtg(const knapsack_t *, ub_t, qft_t, add_t, mc_t, uint8_t);

/*
 * Function: 		gate_count_qtg
 * -------------------------------
 * Description:		This function counts the number of gates used for an application of the QTG.
 *					The implementation of the QTG's subroutines also have to be specified.
 * Parameters:
 *		parameter1:	Pointer to the knapsack that should be considered.
 *		parameter2:	Method how the upper bound should be determined.
 *		parameter3:	Method how the QFT is implemented.
 *		parameter4: Method how the adder is implemented.
 *		parameter5:	Method how the multi-control gate is decomposed.
 *      parameter6: Whether toffoli gates should be decomposed or not.
 * Returns:			Number of gates of one application of the QTG.
 */
count_t gate_count_qtg(const knapsack_t *, ub_t, qft_t, add_t, mc_t, uint8_t);

void
print_qtg_counts(const knapsack_t* k, ub_t method_ub, qft_t method_qft, add_t method_add, mc_t method_mc, uint8_t tof_decomp);

#ifdef __cplusplus
}
#endif
#endif