# Simulator for the Quantum Tree Generator

This repository contains the source code for the simulated Quantum Tree Generator (QTG), described in INSERTPAPERNAME. Please cite this paper as:

INSERTPAPERREFERENCE

The corresponding BibTeX entry is 

INSERTBIBTEXENTRY

## Content

Primarily, this module simulates the application of the QMaxSearch routine to knapsack benchmark instances. For the simulation and further comparison, it contains slightly modified source code for the state-of-the-art exact knapsack solver: [Combo](http://hjemmesider.diku.dk/~pisinger/codes.html).
Further contents are:
- [cmbcount](cmbcount.c): Isolated execution of Combo; compiled version is used for cycle and memory count
- [combowrp](src/combowrp.c): Wrapper for Combo to match remaining module structure
- [knapsack](src/knapsack.c): Provides underlying knapsack data structure, sorting and greedy routines, and benchmark instance formatting
- [main](main.c): Reads knapsack instances to be considered, executes Combo and QMaxSearch, and saves results in files
- [qtgcount](src/qtgcount.c): Evaluates the resources needed for applying the QTG to given knapsack instances
- [simulate](src/simulate.c): Simulates quantum measurements, amplitude amplification, QSearch, and QMaxSearch
- [stategen](src/stategen.c): Simulates the QTG
- [syslinks](src/syslinks.c): Provides OS-dependent functionalities for Windows, Linux and MacOS