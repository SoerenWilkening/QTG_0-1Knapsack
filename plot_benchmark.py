import numpy as np
import matplotlib.pyplot as plt

f = 0.1
eps = 0.1
x = 100
n = 1000
Z = 10000000000
M = 200
states = "comp"
bias = {400: 75,
        600: 112,
        800: 160,
        1000: 200,
        1200: 240}

ram = []
qubits = []
ram_std = []

for f in [0.1, 0.2, 0.3]:
    for eps in [0.1, 0.01, 0.001, 0.0001]:
        for x in [100, 200, 300]:
            filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_" + \
                       str(n) + "_c_" + str(Z) + "_g_2_f_" + \
                       str(f) + "_eps_" + str(eps) + "_s_" + str(x) + "/test.in"

            filename = filename[filename.index("instances_01_KP"):].replace("instances_01_KP", ".calculations")

            data = np.array(list(map(float, open(filename + "/benchmark/combo_ram.txt").read().split()))) / (n + 2 * np.ceil(np.log2(Z)))
            data = data ** (-1)
            ram.append(np.mean(data))
            ram_std.append(np.std(data))

plt.errorbar([i for i in range(len(ram))], ram, fmt=".--", yerr=ram_std, label="Bits")
d, e = plt.xlim()
# plt.hlines(n + 2 * np.ceil(np.log2(Z)), d, e, colors="green", label="Qbits")
a, b = plt.ylim()
# plt.ylim(0, b + 10)
# plt.yscale("log")
plt.ylabel("#QBits / #Bits")
plt.xlabel("Instance")
# plt.legend()
plt.tight_layout()
plt.show()
