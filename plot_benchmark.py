import numpy as np
import matplotlib.pyplot as plt

f = 0.1
eps = 0.1
x = 100
n = 1200
Z = 10000000000
M = 200
states = "comp"
bias = {400: 100,
        600: 150,
        800: 200,
        1000: 250,
        1200: 300}

ram = []
runtime = []
qruntime = []
qubits = []
ram_std = []
runtime_std = []
qruntime_std = []
probs = []

files = []

a = 0
b = 0

# for f in [0.1, 0.2, 0.3]:
#     for eps in [0, 0.1, 0.01, 0.001, 0.0001]:
#         for x in [100, 200, 300]:
for f in [0.1, 0.2, 0.3]:
    for eps in [0, "1e-05"]:
        for x in [100, 200, 300]:

            filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_" + \
                       str(n) + "_c_" + str(Z) + "_g_2_f_" + \
                       str(f) + "_eps_" + str(eps) + "_s_" + str(x) + "/test.in"

            filename = filename[filename.index("instances_01_KP"):].replace("instances_01_KP", ".calculations_old1")
            if "n_1200_c_10000000000_g_2_f_0.3_eps_0_s_300" in filename:
                b = a

            gr = list(map(int, open(filename + "/benchmark/greedy.txt").read().split()))[0]
            zzz = list(map(int, open(filename + "/combo/n_new=" + str(n) + "_Z=" + str(Z) + ".txt").read().split()))[0]

            if gr != zzz:
                files.append(filename)
                a += 1
                data = np.array(list(map(float, open(filename + "/benchmark/combo_ram.txt").read().split()))) \
                       # / (2 * n + 4 * np.ceil(np.log2(Z)))
                # data = data ** (-1)
                ram.append(np.mean(data))
                ram_std.append(np.std(data))

                data = list(map(float, open(filename + "/benchmark/combo_runtime.txt").read().split()))
                runtime.append(np.mean(data))
                runtime_std.append(np.std(data))

                data = open(
                    filename + "/benchmark/qtg/ub=no/bias=" + str(bias[n]) + ".000000/M=200/states=comp/runtime.txt").read().split("\n")
                data.pop()

                counts = []

                ratio = 0

                for dat in data:
                    liste = list(map(int, dat.split()))

                    P = liste[0]
                    gates = liste[1:]
                    # print(gates)
                    if P == zzz:
                        ratio += 1
                        for i in range(3, len(gates)):
                            gates[2] += gates[i] * 2 * (i - 1)
                            gates[1] += gates[i]
                            gates[i] = 0
                        counts.append(gates[0] + gates[1] + gates[2])
                    # print(gates)
                    # print()
                print(len(data))
                probs.append(100 * ratio / len(data))
                qruntime.append(np.mean(counts))
                qruntime_std.append(np.std(counts))

print(b)

print(files)

plt.title("$" + str(n) + "$ variables")
plt.errorbar([i for i in range(len(ram))], ram, fmt=".--", yerr=ram_std, label="Bits")
a,b = plt.xlim()
plt.hlines(2 * n + 4 * np.ceil(np.log2(Z)), a, b, label="Qubits", colors="orange")
plt.xlim(a, b)

plt.ylabel("#Bits")
plt.xlabel("Instance")
plt.yscale("log")
plt.legend()
plt.tight_layout()
plt.show()

plt.title("$" + str(n) + "$ variables")
plt.errorbar([i for i in range(len(runtime))], runtime, fmt=".--", yerr=runtime_std, label="classical", capsize=4)
plt.errorbar([i for i in range(len(qruntime))], qruntime, fmt=".--", yerr=qruntime_std, label="quantum", capsize=4)
for i in range(len(qruntime)):
    plt.text(i, qruntime[i], str(probs[i])[:3], fontdict={"size": 7.5})
plt.xlabel("Instance")
plt.ylabel("elapsed cycles")
plt.legend()
plt.tight_layout()
plt.show()
