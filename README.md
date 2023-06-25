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

Since the entire source code is written in C, a suitable compiler is necessary. The [makefile](makefile) is written to utilize [gcc](https://gcc.gnu.org). The first step is to ensure to have it installed. On MacOS, gcc typically is an alias for [clang](https://clang.llvm.org) which therefore should be installed instead.

This module integrates two external libraries: [GNU Scientific Library (GSL)](https://www.gnu.org/software/gsl/) and [GNU Multiple Precission Arithmetic Library (GMP)](https://gmplib.org) which also have to be installed in advance. On Windows, the installation paths for these libraries have to be given explicitely during the linking process. For this, the [makefile](makefile) contains two variables to which the installation path names should be assigned, respectively.

The functions provided in [knapsack.c](src/knapsack.c) are able to format benchmark instances from two sources: [Pisinger's instances](http://hjemmesider.diku.dk/~pisinger/codes.html) described in [D. Pisinger, Computers & Operations Research 32, 2271 (2005)](https://doi.org/10.1016/j.cor.2004.03.002) and [Jooken et al.'s instances](https://github.com/JorikJooken/knapsackProblemInstances) described in [J. Jooken, P. Leyman, and P. De Causmaecker, European Journal of Operational Research 301, 841 (2022)](https://doi.org/10.1016/j.ejor.2021.12.009). The corresponding directories are assumed to exist within the [instances](instances/) folder, but are not distributed within this repository.
