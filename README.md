# Simulator for the Quantum Tree Generator

This repository contains the source code for the simulated Quantum Tree Generator (QTG), described in "A quantum
algorithm for the solution of the 0-1 Knapsack problem".

## Installation

The code in this repository is mostly implemented in C++. For easier usage we build Python wrappers 
around the code that can be build installed using the package manager pip:

```
pip install .
```

> [!WARNING]  
> For ARM-based architectures, the installation might fail due to the need for x86 assembly.
> The installer will use a "default" conan profile if not configured otherwise.
> Due to the need for x86 assembly, you might have to setup a custom profile in `.conan2/profiles` if you are using an
> ARM Mac.
> Such a profile could look something like this
> ```
> [settings]
> arch=x86_64
> build_type=Release
> compiler=apple-clang
> compiler.cppstd=gnu17
> compiler.libcxx=libc++
> compiler.version=14
> os=Macos
> ```
> The profile can then be set in the `setup.py`.


Once everything is installed properly, you can run the tests to verify the installation.
For this, you need to install the `pytest` package using `pip install pytest`.

```bash
pytest
```

## Running the Code

We implemented several classical solvers and the quantum tree generator.

**Combo**

```python

from qtg.bindings import execute_combo
from qtg.utils import load_instance

instance = load_instance("path/to/instance")
solution = execute_combo(instance)
```

The `solution` contains three values
* `objective_value` The objective value (always optimal)
* `item_assignments` The assignment of items to the knapsack
* `elapsed_cycles` The number of CPU cycles it took to solve the instance.

**Expknap**

```python

from qtg.bindings import execute_expknap
from qtg.utils import load_instance

instance = load_instance("path/to/instance")
solution = execute_expknap(instance)
```

The `solution` contains three values
* `objective_value` The objective value (always optimal)
* `item_assignments` The assignment of items to the knapsack
* `elapsed_cycles` The number of CPU cycles it took to solve the instance.


**IP and CP-SAT**
```python
from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance

instance = load_instance("path/to/instance")
timeout = 60 # time limit in seconds

# set model type to one of ["ip", "cp-sat"]
solver = KnapsackSolver(instance, model_type="ip")
solution = solver.solve(time_limit=timeout)
```

The `solution` contains five values: 
* `objective_value` The objective value
* `item_assignments` The assignment of items to the knapsack
* `best_bound` The best bound found by the solver
* `optimal` `True` if the solution is optimal, `False` otherwise
* `elapsed_time` The time it took to solve the instance.

**QTG**

```python
from qtg.bindings import execute_q_max_search
from qtg.utils import load_instance

instance = load_instance("path/to/instance")
bias = 4
qtg_iterations = 100
seed = 42
solution = execute_q_max_search(instance, bias, qtg_iterations, seed)
```

The `solution` contains seven values:
* `objective_value` The objective value
* `item_assignments` The assignment of items to the knapsack
* `qubit_count` The number of qubits used
* `cycle_count` The number of cycles used
* `gate_count` The number of gates used
* `cycle_count_decomp` The number of cycles used for the decomposition
* `gate_count_decomp` The number of gates used for the decomposition

## Evaluation

All evaluation scripts are located in the `evaluation` folder. The scripts are written in Python and can be run from the
command line. See the following rundown of the folder structure:

* `evaluation/instances` contains the instances used for the evaluation.
    * `hard` contains instances that are hard to solver and thus only used for the comparison of classical algorithms.
    * `easy` contains instances that are easy to solve and thus used for the evaluation of QTG.
* `evaluation/results` contains the results of the evaluation.

## Generate new Instances
New instances can be generated using the instance generator
from [J. Jooken, P. Leyman, and P. De Causmaecker, European Journal of Operational Research 301, 841 (2022)](https://doi.org/10.1016/j.ejor.2021.12.009).
Sample instances were provided by the authors in the
[Jooken et al.'s instance set](https://github.com/JorikJooken/knapsackProblemInstances).
Our instance generator calls Jooken's code to generate new instances and can be called via the command line.

```bash
usage: evaluation/00_generate.py [-h] [-n N] [-Z Z] [-g G] [-f FRAC] [-e EPSILON] [-s SMALL] [-o OUT]

options:
  -h, --help            show this help message and exit
  -n N
  -Z Z
  -g G
  -f FRAC, --frac FRAC
  -e EPSILON, --epsilon EPSILON
  -s SMALL, --small SMALL
  -o OUT, --out OUT
```

There are other instance sets like the
[Pisinger's instances](http://hjemmesider.diku.dk/~pisinger/codes.html) described
in [D. Pisinger, Computers & Operations Research 32, 2271 (2005)](https://doi.org/10.1016/j.cor.2004.03.002)
which are not part of this work.

## Cite this work

Please cite this paper as:

> Wilkening, Sören, et al. "A quantum algorithm for the solution of the 0-1 Knapsack problem." arXiv preprint arXiv:
2310.06623 (2023).

The corresponding BibTeX entry is

```bibtex
@article{wilkening2023quantum,
  title={A quantum algorithm for the solution of the 0-1 Knapsack problem},
  author={Wilkening, S{\"o}ren and Lefterovici, Andreea-Iulia and Binkowski, Lennart and Perk, Michael and Fekete, S{\'a}ndor and Osborne, Tobias J},
  journal={arXiv preprint arXiv:2310.06623},
  year={2023}
}
```
