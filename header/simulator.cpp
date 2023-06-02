//
// Created by Sören Wilkening on 11.05.23.
//

#include "simulator.h"


std::vector<state_node> QMaxSearch::amplitude_amplification(long threshold, int calls) {

    /* at this point bnb is always empty, but in a later stage maybe not */
    if (bnb.empty()) bnb = breadth_first_search(data, threshold, exact, bias, states, presious_sol);

    std::vector<state_node> sample; // result will contain a single state -> the result of simulate

    double total, factor;

    total = 0;
    for (auto &i: bnb) if (threshold < i.P) total += pow(i.amplitude, 2);

    std::vector<double> probabilities(bnb.size() + 1, 0);

    factor = pow(sin((2 * calls + 1) * asin(sqrt(total))), 2) / total;

    int i;
    total = 0;
    for (i = 0; i < bnb.size(); ++i) {
        if (threshold < bnb[i].P) {
            probabilities[i] = pow(bnb[i].amplitude, 2) * factor;
            total += probabilities[i];
        }
    }
    probabilities[i] = 1. - total;

    /* executing a quantum measurement */
    int measurement = sampling(&probabilities); // sampling from the probability distribution
    sample.clear();
    sample.shrink_to_fit();
    if (measurement == bnb.size()) {
        sample.push_back({0, 0, 0, 0, 0});
    }
    else {
        sample.push_back({bnb[measurement].P, 0, 0, 0, 1});
        mpz_set(sample[0].vector, bnb[measurement].vector);
    }

    probabilities.clear();
    probabilities.shrink_to_fit();
    return sample;
}

std::vector<state_node> QMaxSearch::QSearch(long threshold, int M) {
    int l = 0, m_tot = 0, m, j;
    double c = 6. / 5;

    std::vector<state_node> sample;

    while(m_tot < M){
        m = ceil(pow(c, l));
        j = rand() % m;
        m_tot += 2 * j + 1;

        sample.clear();
        sample.shrink_to_fit();
        sample = QMaxSearch::amplitude_amplification(threshold, j);
//        gates_amplitude_amplification(&data, &gates, &qtg, j, threshold);
        // count cycles
        gates[0] += (2 * j + 1) * cycle_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 0); // without decomposing toffolies
        gates[1] += (2 * j + 1) * cycle_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 1); // with decomposing toffolies

        // count gates
        gates[2] += (2 * j + 1) * gate_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 0); // without decomposing toffolies
        gates[3] += (2 * j + 1) * gate_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 1); // with decomposing toffolies

        gates[0] += cycle_count_comp(profit_reg_size(k, FGREEDY),sample[0].P + 1, TOFFOLI, 1, 0); // without decomposing toffolies
        gates[1] += cycle_count_comp(profit_reg_size(k, FGREEDY),sample[0].P + 1, TOFFOLI, 1, 1); // with decomposing toffolies
        gates[0] += cycle_count_mc(data.n - 1, TOFFOLI, 0); // without decomposing toffolies
        gates[1] += cycle_count_mc(data.n - 1, TOFFOLI, 1); // with decomposing toffolies

        gates[2] += gate_count_comp(profit_reg_size(k, FGREEDY),sample[0].P + 1, TOFFOLI, 1, 0); // without decomposing toffolies
        gates[3] += gate_count_comp(profit_reg_size(k, FGREEDY),sample[0].P + 1, TOFFOLI, 1, 1); // with decomposing toffolies
        gates[2] += gate_count_mc(data.n - 1, TOFFOLI, 0); // without decomposing toffolies
        gates[3] += gate_count_mc(data.n - 1, TOFFOLI, 1); // with decomposing toffolies

        if (sample[0].P > threshold){
            M_tot += m_tot;
            return sample;
        }
        l++;
    }
    M_tot += m_tot;
    return std::vector<state_node>(1, {0, 0});
}

std::vector<state_node> QMaxSearch::execute(int M) {
    M_tot = 0;
    qtg = qtg_gatecount(data);
    qtg2.resize(4);
    gates.resize(qtg.size());
    gates2.resize(qtg2.size());

//    int a = 0;
//    long total;
//    for (auto &i: qtg) {
//        if(a > 2) {
////            std::cout << i * 2 * (a - 1) << std::endl;
//            total += i * 2 * (a - 1);
//        }
//        else {
////            std::cout << i << std::endl;
//            total += i;
//        }
//        a++;
//    }
//
//    std::cout << std::endl;
//    std::cout << "Total number of cycles (Sören) without decomposing Toffolis:\n" << total << std::endl;
//    std::cout << std::endl;

    int break_item = 0;
    double upper_bound = 0;
    std::vector<state_node> sample = greedy(data.n, data.Z, data.p, data.z, 0, &break_item, &upper_bound);
    std::vector<state_node> measured;

    mpz_set(presious_sol, sample[0].vector);


    for (int i = 0; i < data.n; ++i) {
        k->items[i].cost = data.z[i];
        k->items[i].profit = data.p[i];
    }


    qtg2[0] = cycle_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 0); // without decomposing toffolies
    qtg2[1] = cycle_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 1); // with decomposing toffolies

    qtg2[2] = gate_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 0); // without decomposing toffolies
    qtg2[3] = gate_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, TOFFOLI, 1); // with decomposing toffolies
//    std::cout << cycle_count_comp(profit_reg_size(k, FGREEDY),sample[0].P + 1, TOFFOLI, 1, 0) << std::endl;
//    std::cout << cycle_count_mc(data.n - 1, TOFFOLI, 0) << std::endl;
//
//    std::cout << gate_count_comp(profit_reg_size(k, FGREEDY),sample[0].P + 1, TOFFOLI, 1, 1) << std::endl;
//    std::cout << gate_count_mc(data.n - 1, TOFFOLI, 1) << std::endl;

//    int x = 5;
//    std::cout << (((x >> 0) & 1) == 0) << std::endl;
//    std::cout << (((x >> 1) & 1) == 0) << std::endl;
//    std::cout << (((x >> 2) & 1) == 0) << std::endl;

    while (true) {
        measured = QSearch(sample[0].P, M);
        if (measured[0].P > sample[0].P) {
            sample.clear();
            sample[0] = measured[0];
            mpz_set(presious_sol, measured[0].vector);
//            std::cout << "P = " << sample[0].P << std::endl;
            bnb.clear();
            bnb.shrink_to_fit();
        }
        else {
//            fs::create_directories(data.name + "/benchmark/qtg/ub=" + ub + "/bias=" + std::to_string(bias) + "/M=" + std::to_string(M) + "/states=" + states + "/");
//            std::ofstream myfile(data.name + "/benchmark/qtg/ub=" + ub + "/bias=" + std::to_string(bias) + "/M=" + std::to_string(M) + "/states=" + states + "/runtime.txt", std::ios::app);
//            myfile << sample[0].P << " ";
//            for (auto &i:gates) myfile << i << " ";
//            myfile << std::endl;
//
//            myfile.close();
            return sample;
        }
        measured.clear();
    }
}