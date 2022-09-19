//
// Created by ishuena on 19.10.17.
//
#ifndef HEINZ_CCUNROOTED_H
#define HEINZ_CCUNROOTED_H

#include <mwcspreprocessedgraph.h>
#include <ctime>
#include <algorithm>
#include <vector>
#include <cfloat>

namespace colcod {
    class CcUnrooted {

    public:
        typedef typename std::vector<std::vector<std::vector<double>>> Table;
        typedef typename std::pair<std::string, double> CcResult; //declaration

        CcUnrooted() {
        }

        virtual ~CcUnrooted() {
        }

    private:
        double best_weight;
        std::string best_graph;

    public:
        const time_t ctt = time(0);
        //int number_of_nodes;
        int k;
        uint32_t basic_colors;
        int subset_start;

        void print_graph(std::vector<double> nodes, std::vector<std::string> labels, std::vector<uint32_t> node_colors,
                         std::vector<std::vector<int>> neighbors) {
            for (int i = 0; (unsigned) i < nodes.size(); i++) {
                std::cout << labels[i] << ": " << "color: " << color_string(node_colors[i]) << " weight: " << nodes[i]
                          << " neigbors: ";
                for (int j = 0; (unsigned) j < neighbors[i].size(); j++) {
                    std::cout << labels[neighbors[i][j]] << " ";
                }
                std::cout << std::endl;
            }

        }

        std::string color_string(uint32_t src) {
            std::string result = "";
            for (int i = 1; i <= 32; i++) {
                if ((unsigned) 1 << (i - 1) == (src & (1 << (i - 1)))) {
                    result += std::to_string(i) + " ";
                }
            }
            return result;
        }


        void print_table(Table table, std::vector<std::string> labels, std::vector<uint32_t> colorset, CcResult &result) {
            std::cout << "----------------- START TABLE ---------------------" << std::endl;
            for (int i = 0; (unsigned) i < table.size(); i++) {
                for (int j = 0; (unsigned) j < table[i].size(); j++) {
                    //Hier
                    //if (table[i][j][0] == -DBL_MAX) {
                    //    std::cout << " leeg ";
                    //} else {
                    if (!table[i][j].empty()) {
                        std::cout << "Node " << labels[i] << " colors: " << color_string(colorset[j]);
                        std::cout << " weight: " << table[i][j][0];
                        std::string path = "";
                        for (int l = 1; (unsigned) l < table[i][j].size(); l++) {
                            path += labels[table[i][j][l]];
                            path += " ";
                        }
                        std::cout << " with: " << path;
                        if (table[i][j][0] > best_weight) {
                            best_weight = table[i][j][0];
                            best_graph = path;
                        }
                        std::cout << std::endl;
                    }
                }
            }
            std::cout << "----------------- END TABLE ---------------------" << std::endl;
            std::cout << "Max weight: " << best_weight << " with path " << best_graph << std::endl;

            result.first = best_graph;
            result.second = best_weight;
        }

        int binomial_coeff(int n, int k) {
            int res = 1;

            if (k > n - k) k = n - k;

            for (int i = 0; i < k; ++i) {
                res *= (n - i);
                res /= (i + 1);
            }
            return res;
        }

        // colorset bithacks
        int count_ones(uint32_t i) {
            i = i - ((i >> 1) & 0x55555555);
            i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
            return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
        }

        void create_subsets(uint32_t src, int size, std::vector<uint32_t> *dest) {
            uint32_t current = 0;
            for (int i = 0; i < k; i++) {
                get_subsets(i, get_next(0, src), src, current, dest);
            }
        }

        void get_subsets(int size, int start, uint32_t src, uint32_t current, std::vector<uint32_t> *result) {
            if (count_ones(current) == size) {
                if (std::find(result->begin(), result->end(), current) == result->end()){
                    result->push_back(current);
                }
                return;
            }
            if (start >= 32) return;

            current = current | (1 << (start - 1));
            get_subsets(size, get_next(start + 1, src), src, current, result);
            current = (current & ~(1 << (start - 1)));
            get_subsets(size, get_next(start + 1, src), src, current, result);
        }

        int get_next(int start, uint32_t src) {
            while ((unsigned) (1 << (start - 1)) != (src & (1 << (start - 1)))) {
                start++;
            }
            return start;
        }

        void init_colorset(std::vector<uint32_t> *colorset) {
            uint32_t temp = 0;
            basic_colors = 0;
            for (int i = 1; i <= k; i++) {
                basic_colors = (basic_colors | (1 << (i - 1)));
            }
            for (int i = 1; i <= k; i++) {
                get_subsets(i, get_next(0, basic_colors), basic_colors, temp, colorset);
            }
        }

        void create_colors(std::vector<u_int32_t> *node_colors, int nodes, int k) {
            for (int i = 0; i < nodes; i++) {
                int random = (rand() % k) + 1;
                uint32_t color = 0;
                color = (color | (1 << (random - 1)));
                node_colors->push_back(color);
            }
        }

        void init_table(Table *table, std::vector<double> nodes, std::vector<u_int32_t> node_colors) {

            for (int i = 0; (unsigned) i < nodes.size(); i++) {
                std::vector<std::vector<double>> temp_colors;
                temp_colors.reserve(pow(2,k) - 1);
                for (int j = 1; j <= k; j++) {
                    std::vector<double> weight;

                    if((unsigned) (1 << (j - 1)) == (node_colors[i] & (1 << (j - 1)))) {
                        weight.push_back(nodes[i]);
                        weight.push_back(i);
                    } else {
                        // Hier
                        //weight.push_back(-DBL_MAX);
                    }
                    temp_colors.push_back(weight);
                }
                table->push_back(temp_colors);
            }
        }

        CcResult run(std::vector<double> nodes, std::vector<std::string> labels, std::vector<std::vector<int>> neighbors, int number_of_colors, int number_of_iterations) {
            std::cout << "UNROOTED VERSION OF COLOR CODING \n";
            CcResult result; //declaration
            best_weight = -DBL_MAX;
            best_graph = "";
            k = subset_start = number_of_colors;
            basic_colors = 0;

            std::vector<u_int32_t> colorset;


            colorset.reserve(pow(2,k) - 1);
            init_colorset(&colorset);

            // iterations
            for (int iter = 0; iter < number_of_iterations; iter++) {
                std::cout << "It: " << iter << " " ; 

                int nr_of_subsets = 0;
                subset_start = k;

                // setup data structure
                std::vector<uint32_t> node_colors;
                node_colors.reserve(nodes.size());
                Table table;
                table.reserve(nodes.size());

                create_colors(&node_colors, nodes.size(), number_of_colors);
                init_table(&table, nodes, node_colors);


                // for each, color i among k colors do
                for (int i = 2; i <= k; i++) {

                    // create all possible sub color sets of size i
                    subset_start += nr_of_subsets;
                    nr_of_subsets = binomial_coeff(k, i);

                    // for each vertex u in V do
                    for (int current_node = 0; (unsigned) current_node < nodes.size(); current_node++) {
                        // for each colorset T of size i do
                        for (int t = subset_start; t < (subset_start + nr_of_subsets); t++) {
                            std::vector<double> best;
                            //Hier
                            //best.push_back(-DBL_MAX);

                            // is kleur van v in t?

                            if ((unsigned) (1 << (node_colors[current_node])) != (colorset[t] & (node_colors[current_node]))) {

                                for (int current_neighbor = 0;
                                     (unsigned) current_neighbor < neighbors[current_node].size(); current_neighbor++) {

                                    // is kleur van neighbor u in t-v?
                                    if ((unsigned) (1 << (node_colors[neighbors[current_node][current_neighbor]])) !=
                                            (colorset[t] & (node_colors[neighbors[current_node][current_neighbor]]))
                                        && node_colors[current_node] != node_colors[current_neighbor]) {

                                        //for each colorset Q s.t. Q is subset of T AND Q != empty
                                        std::vector<uint32_t> q_subsets_t;
                                        create_subsets(colorset[t], t, &q_subsets_t);
                                        for (int q = 1; (unsigned) q < q_subsets_t.size(); q++) {
                                            // R = T / Q
                                            uint32_t r_subset_t = colorset[t] & (~q_subsets_t[q]);

                                            // is kleur van v element van subset?
                                            if ((unsigned) (1 << (node_colors[current_node])) !=
                                                (q_subsets_t[q] & (node_colors[current_node]))) {

                                                // ja -> is kleur van neighbor element van r=t-q?
                                                if ((unsigned) (1 << (node_colors[neighbors[current_node][current_neighbor]])) !=
                                                          (r_subset_t &
                                                           (node_colors[neighbors[current_node][current_neighbor]]))) {

                                                    std::vector<double> candidate;
                                                    candidate.push_back(-DBL_MAX);
                                                    std::vector<double> first;
                                                    std::vector<double> second;
                                                    for (int index = 0; (unsigned) index < table[current_node].size(); index++) {
                                                        if (colorset[index] == q_subsets_t[q]) {
                                                            //Hier
                                                            if (!table[current_node][index].empty()) {
                                                                candidate[0] = table[current_node][index][0];
                                                            }
                                                            //candidate[0] = table[current_node][index][0];
                                                            first = table[current_node][index];
                                                        }
                                                    }
                                                    for (int indexn = 0; (unsigned) indexn <
                                                                         table[neighbors[current_node][current_neighbor]].size(); indexn++) {
                                                        if (colorset[indexn] == r_subset_t) {
                                                            //Hier
                                                            if (!table[neighbors[current_node][current_neighbor]][indexn].empty()) {
                                                                candidate[0] += table[neighbors[current_node][current_neighbor]][indexn][0];
                                                            }
                                                            //candidate[0] += table[neighbors[current_node][current_neighbor]][indexn][0];
                                                            second = table[neighbors[current_node][current_neighbor]][indexn];
                                                        }
                                                    }
                                                    // If weight is better than best, pushback colorset and path
                                                    if (candidate[0]!= -DBL_MAX && (best.empty() || candidate[0] > best[0])) {
                                                        //if (candidate[0] > best[0]) {
                                                        best = candidate;
                                                        for (int ifi = 1; (unsigned) ifi < first.size(); ifi++) {
                                                            best.push_back(first[ifi]);
                                                        }
                                                        for (int is = 1; (unsigned) is < second.size(); is++) {
                                                            if (std::find(best.begin(), best.end(), second[is]) ==
                                                                best.end()) {
                                                                best.push_back(second[is]);
                                                            }
                                                        }

                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                table[current_node].push_back(best);

                            }
                            //table[current_node].push_back(best);
                        }
                    }
                }
                // verander naam
                print_table(table, labels, colorset, result);
            }
            return result;
        }
    };
}
#endif //HEINZ_CCUNROOTED_H
