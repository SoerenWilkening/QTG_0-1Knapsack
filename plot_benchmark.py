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


files = []


# for f in [0.1, 0.2, 0.3]:
#     for eps in [0, 0.1, 0.01, 0.001, 0.0001]:
#         for x in [100, 200, 300]:
def classical():
    counts = {}
    a = 0
    ram = []
    ram_std = []
    runtime = []
    runtime_wsol = []
    runtime_std = []
    runtime_wsol_std = []
    for s in [2, 6, 10]:
        counts[s] = 0
        for f in [0.1, 0.2, 0.3]:
            for eps in [0, "1e-05"]:
                for x in [100, 200, 300]:

                    try:
                        filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_" + \
                                   str(n) + "_c_" + str(Z) + "_g_" + str(s) + "_f_" + \
                                   str(f) + "_eps_" + str(eps) + "_s_" + str(x) + "/test.in"

                        filename = filename[filename.index("instances_01_KP"):].replace("instances_01_KP",
                                                                                        ".calculations")
                        if "n_1200_c_10000000000_g_2_f_0.3_eps_0_s_300" in filename:
                            b = a

                        gr = list(map(int, open(filename + "/benchmark/greedy.txt").read().split()))[0]
                        zzz = list(map(int, open(
                            filename + "/combo/n_new=" + str(n) + "_Z=" + str(Z) + ".txt").read().split()))[0]

                        if gr != zzz:
                            files.append(filename)
                            a += 1
                            data = np.array(
                                list(map(float, open(filename + "/benchmark/combo_ram.txt").read().split()))) \
                                # / (2 * n + 4 * np.ceil(np.log2(Z)))
                            # data = data ** (-1)
                            ram.append(np.mean(data))
                            ram_std.append(np.std(data))

                            data = list(map(float, open(filename + "/benchmark/combo_runtime.txt").read().split()))
                            runtime.append(np.mean(data))
                            runtime_std.append(np.std(data))
                            counts[s] += 1
                    except:
                        continue
    for s in [2, 6, 10]:
        # counts[s] = 0
        for f in [0.1, 0.2, 0.3]:
            for eps in [0, "1e-05"]:
                for x in [100, 200, 300]:

                    try:
                        filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_" + \
                                   str(n) + "_c_" + str(Z) + "_g_" + str(s) + "_f_" + \
                                   str(f) + "_eps_" + str(eps) + "_s_" + str(x) + "/test.in"

                        filename = filename[filename.index("instances_01_KP"):].replace("instances_01_KP",
                                                                                        ".calculations")
                        if "n_1200_c_10000000000_g_2_f_0.3_eps_0_s_300" in filename:
                            b = a

                        gr = list(map(int, open(filename + "/benchmark/greedy.txt").read().split()))[0]
                        zzz = list(map(int, open(
                            filename + "/combo/n_new=" + str(n) + "_Z=" + str(Z) + ".txt").read().split()))[0]

                        if gr != zzz:
                            # files.append(filename)
                            a += 1
                            # data = np.array(
                            #     list(map(float, open(filename + "/benchmark/combo_ram.txt").read().split()))) \
                                # / (2 * n + 4 * np.ceil(np.log2(Z)))
                            # data = data ** (-1)
                            # ram.append(np.mean(data))
                            # ram_std.append(np.std(data))

                            data = list(map(float, open(filename + "/benchmark/combo_runtime_with_sol.txt").read().split()))
                            runtime_wsol.append(np.mean(data))
                            runtime_wsol_std.append(np.std(data))
                            # counts[s] += 1
                    except:
                        continue

    return ram, ram_std, runtime, runtime_std, counts, runtime_wsol, runtime_wsol_std

def quantum():
    counts = {}
    a = 0
    qruntime = []
    qruntime_std = []
    probs = []
    for s in [2]:
        counts[s] = 0
        for f in [0.1, 0.2, 0.3]:
            for eps in [0, "1e-05"]:
                for x in [100, 200, 300]:

                    try:
                        filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_" + \
                                   str(n) + "_c_" + str(Z) + "_g_" + str(s) + "_f_" + \
                                   str(f) + "_eps_" + str(eps) + "_s_" + str(x) + "/test.in"

                        filename = filename[filename.index("instances_01_KP"):].replace("instances_01_KP",
                                                                                        ".calculations")
                        if "n_1200_c_10000000000_g_2_f_0.3_eps_0_s_300" in filename:
                            b = a

                        gr = list(map(int, open(filename + "/benchmark/greedy.txt").read().split()))[0]
                        zzz = list(map(int, open(
                            filename + "/combo/n_new=" + str(n) + "_Z=" + str(Z) + ".txt").read().split()))[0]

                        if gr != zzz:
                            files.append(filename)
                            a += 1
                            data = np.array(
                                list(map(float, open(filename + "/benchmark/combo_ram.txt").read().split()))) \
                                # / (2 * n + 4 * np.ceil(np.log2(Z)))
                            # data = data ** (-1)
                            # ram.append(np.mean(data))
                            # ram_std.append(np.std(data))

                            data = list(map(float, open(filename + "/benchmark/combo_runtime.txt").read().split()))
                            # runtime.append(np.mean(data))
                            # runtime_std.append(np.std(data))
                            counts[s] += 1

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
                    except:
                        continue

    return qruntime, qruntime_std, probs





ram, ram_std, runtime, runtime_std, counts, runtime_wsol, runtime_wsol_std = classical()
qruntime, qruntime_std, probs = quantum()


# print(counts)

# print(b)

# print(files)

plt.title("$" + str(n) + "$ variables")
plt.errorbar([i for i in range(len(ram))], ram, fmt=".--", yerr=ram_std, label="Bits")
a, b = plt.xlim()
plt.hlines(2 * n + 4 * np.ceil(np.log2(Z)), a, b, label="Qubits", colors="orange")
plt.xlim(a, b)
plt.yscale("log")
a, b = plt.ylim()
plt.fill_betweenx([a, b], -10, counts[2] - 1, color="green", alpha=.2)
# plt.vlines(counts[2] - 1, a, b)
plt.ylim(a, b)

plt.ylabel("#Bits")
plt.xlabel("Instance")
plt.legend()
plt.tight_layout()
plt.savefig("num_bit.pdf")
plt.show()

from matplotlib.ticker import MaxNLocator

ax = plt.figure().gca()

ax.set_title("$" + str(n) + "$ variables")
ax.errorbar([i for i in range(len(runtime[:counts[2]]))], runtime[:counts[2]], color="tab:blue", fmt=".--", yerr=runtime_std[:counts[2]], label="combo", capsize=4, ecolor="tab:blue")
ax.errorbar([i for i in range(len(runtime_wsol[:counts[2]]))], runtime_wsol[:counts[2]], color="tab:green", fmt=".--", yerr=runtime_wsol_std[:counts[2]], label="combo + define solution", capsize=4, ecolor="tab:green")
ax.errorbar([i for i in range(len(qruntime))], qruntime, color="tab:orange", fmt=".--", yerr=qruntime_std, label="quantum", capsize=4, ecolor="tab:orange")
for i in range(len(qruntime)):
    ax.text(i, qruntime[i], str(probs[i])[:3], fontdict={"size": 7.5})
# plt.xlabel("Instance")
ax.set_ylabel("elapsed cycles")
ax.legend()
ax.xaxis.set_major_locator(MaxNLocator(integer=True))
# plt.yscale("log")
plt.tight_layout()
plt.savefig("runtimes.pdf")
plt.show()
