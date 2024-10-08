import argparse

from qtg.bindings import jooken_generate
from qtg.utils import save_instance, load_instance

# n_1600_c_10000000000_g_2_f_0.3_eps_1e-05_s_300
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", type=int, default=10)

    parser.add_argument("-Z", type=int, default=1_000_000)
    parser.add_argument("-g", type=int, default=2)
    parser.add_argument("-f", "--frac", type=float, default=0.3)
    parser.add_argument("-e", "--epsilon", type=float, default=0)
    parser.add_argument("-s", "--small", type=int, default=100)
    parser.add_argument("-o", "--out")

    args = parser.parse_args()

    for n in range(5, 100, 5):
        for g in range(3, 11):  # [2]:
            for eps in [0, 1e-05]:
                for small in [100, 200, 300]:
                    knapsack = jooken_generate(n, args.Z, g, args.frac, eps, small)
                    knapsack.name = knapsack.name.replace("f_0.30", "f_0.3")
                    knapsack.name = knapsack.name.replace("0.00000", "0")
                    knapsack.name = knapsack.name.replace("0.00001", "1e-05")
                    save_instance(knapsack, args.out)
                    print("Crated instance", knapsack.name)
