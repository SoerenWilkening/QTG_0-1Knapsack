import argparse
import sys
import os

sys.path.append(os.path.abspath('../pysrc'))

from qtg.bindings import jooken_generate
from qtg.utils import save_instance, load_instance
#n_1600_c_10000000000_g_2_f_0.3_eps_1e-05_s_300
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", type=int, default=10)
    parser.add_argument("-Z", type=int, default=100000)
    parser.add_argument("-g", type=int, default=2)
    parser.add_argument("-f", "--frac", type=float, default=0.3)
    parser.add_argument("-e", "--epsilon", type=float, default=0)
    parser.add_argument("-s", "--small", type=int, default=100)
    parser.add_argument("-o", "--out")

    args = parser.parse_args()

    knapsack = jooken_generate(args.n, args.Z, args.g, 0.3, args.epsilon, args.small)

    save_instance(knapsack, args.out)
    print("Crated instance", knapsack.name)

