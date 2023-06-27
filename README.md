# Simulator for the Quantum Tree Generator

This repository contains the source code for the simulated Quantum Tree Generator (QTG), described in INSERTPAPERNAME. Please cite this paper as:

INSERTPAPERREFERENCE

The corresponding BibTeX entry is 

INSERTBIBTEXENTRY

## Content

Primarily, this module simulates the application of the Quantum Tree generator and the QMaxSearch routine to knapsack benchmark instances. For the simulation and further comparison, it contains slightly modified source code for the state-of-the-art exact knapsack solver: [Combo](http://hjemmesider.diku.dk/~pisinger/codes.html).
Further contents are:
- [cmbcount](cmbcount.c): Isolated execution of Combo; compiled version is used for cycle and memory count
- [combowrp](src/combowrp.c): Wrapper for Combo to match module structure
- [knapsack](src/knapsack.c): Provides underlying knapsack data structure, sorting and greedy routines, and benchmark instance formatting
- [main](main.c): Reads knapsack instances to be considered, executes Combo and QMaxSearch, and saves results in files
- [makefile](makefile): Compiles the entire project and creates the cmbcount and main executables
- [qtgcount](src/qtgcount.c): Evaluates the resources needed for applying the QTG to given knapsack instances
- [simulate](src/simulate.c): Simulates quantum measurements, amplitude amplification, QSearch, and QMaxSearch
- [stategen](src/stategen.c): Simulates the QTG
- [syslinks](src/syslinks.c): Provides OS-dependent functionalities for Windows, Linux, and MacOS

## Preparation

Since the entire source code is written in C, a suitable compiler is necessary. The [**makefile**](makefile) is written to utilize [gcc](https://gcc.gnu.org). The first step is to ensure to have it installed. On MacOS, gcc typically is an alias for [clang](https://clang.llvm.org) which therefore should be installed instead.

This module integrates two external libraries: [GNU Scientific Library (GSL)](https://www.gnu.org/software/gsl/) and [GNU Multiple Precission Arithmetic Library (GMP)](https://gmplib.org) which also have to be installed in advance. On Windows, the installation paths for these libraries have to be given explicitely during the linking process. For this, the [**makefile**](makefile) contains two variables to which the installation path names should be assigned, respectively.

```
GSLPATH = path/to/gsl/directory
GMPPATH = path/to/gmp/directory
```

The functions provided in [**knapsack.c**](src/knapsack.c) are able to format benchmark instances from two sources: [Pisinger's instances](http://hjemmesider.diku.dk/~pisinger/codes.html) described in [D. Pisinger, Computers & Operations Research 32, 2271 (2005)](https://doi.org/10.1016/j.cor.2004.03.002) and [Jooken et al.'s instances](https://github.com/JorikJooken/knapsackProblemInstances) described in [J. Jooken, P. Leyman, and P. De Causmaecker, European Journal of Operational Research 301, 841 (2022)](https://doi.org/10.1016/j.ejor.2021.12.009). The corresponding directories are assumed to exist within a folder called **instances**, but are not distributed within this repository.

## Execution

After following all prepration instructions, move to the root directory of this repository and then compile its conent using `make`. The [**main.c**](main.c) file reads the instances to be considered from a file called **benchmark_instances.txt** within the same directory. This file should contain the relative paths to the instances to be considered - one instance per line - and should end with a line break.

```
instances/problemInstances/n_400_c_10000000000_g_2_f_0.3_eps_0_s_100/test.in
instances/largecoeff_pisinger/knapPI_4_50_100000.csv
instances/problemInstances/n_1200_c_10000000000_g_2_f_0.3_eps_0.1_s_100/test.in

```

After the compiling process call and the preparation of **benchmark_instances.txt**, call the main executable using `.\main bias qsearch_iter reps` on Windows or `./main bias qsearch_iter reps` on Linux and MacOS. Replace `bias` by a non-negative integer value for the bias that should be applied during the QTG. `qsearch_iter` is to be replaced by a non-negative integer value for a loose upper bound on the calls to the QTG within the QSearch routine. Lastly, `reps` should be replaced by a non-negative integer value for the number of repeating the execution of Combo and QMaxSearch (together with their resource estimation) on each instance.

During the execution, the following data is created and stored:
- optimal values Combo achieves on all relevant subinstances (stored in `ìnstance_name`/combo/size=`n`_capacity=`c`.txt, where `n` and `c`are the instances number of items and capacity, respectively)
- peak memory usage and elapsed cycles of Combo when only calculating the optimal value (stored in ìnstance_name`/combo/combo_counts_def=false.csv)
- peak memory usage and elapsed cycles of Combo when also finding a solution vector (stored in ìnstance_name`/combo/combo_counts_def=true.csv)
- number of qubits, necessary quantum cycles and gates with and without decomposing arising Toffoli gates, and whether optimal value was achieved by QMaxSearch (stored in `instance_name`/QTG/qtg_statistics_bias=`bias`_maxiter=`qsearch_iter`.csv)

## Generate new instances

New instances can be generated using the instance generator by Jooken et al. with the command

`make generate`

THe methods reads the `generator_input.txt` file, that has to be in the format

```
n
c
g
f
eps
s
```
