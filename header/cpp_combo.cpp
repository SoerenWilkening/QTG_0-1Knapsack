//
// Created by Sören Wilkening on 10.05.23.
//

#include "cpp_combo.h"

bool fileExists(const std::string &filename) {
    std::ifstream file(filename);
    return file.good();
}

long cpp_combo_wrap(int n, std::vector<long> p, std::vector<long> w, long c,
                    std::string name,
                    double *timerecord,
                    int first_item,
                    int define_sol,
                    bool read) {
    long P = 0, zzz;
    int res = knapsack_instance_is_trivial(p, w, c, &P, first_item);
    if (res) return P;

    fs::path myFilePath = name + "/combo/n_new=" + std::to_string(n - first_item) + "_Z=" + std::to_string(c) + ".txt";

    if (fileExists(myFilePath) && read) {
        FILE *file = fopen(myFilePath.c_str(), "r");
        fscanf(file, "%ld", &zzz);
        fclose(file);
        return zzz;
    }

    zzz = combo_wrap(n, p.data(), w.data(), c, timerecord, first_item, define_sol);
    if (*timerecord > .0003) {
        fs::create_directories(myFilePath.parent_path());
        std::ofstream myfile(myFilePath);
        if (myfile.is_open()) {
            myfile << zzz << std::endl;
            myfile.close();
        } else std::cout << "Failed to open file.\n";
    }

    return zzz;
}