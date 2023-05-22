//
// Created by Sören Wilkening on 10.05.23.
//

#include "cpp_combo.h"

bool fileExists(const std::string &filename) {
    std::ifstream file(filename);
    return file.good();
}

uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
            "rdtsc" : "=a" (lo), "=d" (hi)
            );
    return ((uint64_t)hi << 32) | lo;
}

long cpp_combo_wrap(int n, std::vector<long> p, std::vector<long> w, long c,
                    std::string name,
                    long *timerecord,
                    int first_item,
                    int define_sol,
                    bool read,
                    bool exec_combo) {

    long P = 0, zzz;
    /* we fist check  i the instance is triviel. If it is, combo runs into problems */
    int res = knapsack_instance_is_trivial(p, w, c, &P, first_item);
    if (res) return P;

    /* if it is not triviel, we check, if we have already caclulated and saved the combo output */
    fs::path myFilePath = name + "/combo/n_new=" + std::to_string(n - first_item) + "_Z=" + std::to_string(c) + ".txt";

    if (fileExists(myFilePath) && read) {
        FILE *file = fopen(myFilePath.c_str(), "r");
        fscanf(file, "%ld", &zzz);
        fclose(file);
        return zzz;
    }

    /* if it is not saved, we execute combo */
    item *a;
    a = (item *) calloc(n, sizeof(item));
    item *f = &a[first_item];
    item *l = &a[n - 1];
    item *j;

    int i = first_item;
    for (j = f; j <= l; j++) {
        j->p = p[i];
        j->w = w[i];
        i++;
    }
    //Can I simply define the initial lower bound as zero?
    int lbi = 0;
    boolean def = define_sol;
    boolean relx = false;
    long ubi;
    ubi = 0;

    if(exec_combo) {
        uint64_t start = rdtsc();

        zzz = combo(f, l, c, lbi, ubi, def, relx, nullptr); //zzz is the solution from the algorithm

        uint64_t end = rdtsc();
        uint64_t elapsed_cycles = end - start;

        *timerecord = elapsed_cycles;

        /* if combo took more than .0003 seconds, we save the result */
        if ((double) *timerecord / (2.6 * pow(10, 9)) > .0003) {
            fs::create_directories(myFilePath.parent_path());
            std::ofstream myfile(myFilePath);
            if (myfile.is_open()) {
                myfile << zzz << std::endl;
                myfile.close();
            } else std::cout << "Failed to open file.\n";
        }
    }
    return zzz;
}