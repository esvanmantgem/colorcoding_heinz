//
// Created by ishuena on 30.04.18.
//

#ifndef HEINZ_COLORCODINGROOTED_H
#define HEINZ_COLORCODINGROOTED_H
#include <mwcspreprocessedgraph.h>
#include <ctime>
#include <algorithm>
#include <vector>
#include <cfloat>
//#include "ColorCoding.h"

namespace colcod {
    template<typename GR,
            typename NWGHT = typename GR::template NodeMap<double>,
            typename NLBL = typename GR::template NodeMap<std::string>,
            typename EWGHT = typename GR::template EdgeMap<double>>
    class ColorCodingRooted {

    public:
        typedef GR Graph;
        typedef NWGHT WeightNodeMap;
        typedef NLBL LabelNodeMap;
        typedef EWGHT WeightEdgeMap;

        typedef nina::mwcs::MwcsPreprocessedGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsPreGraphType;
        typedef nina::mwcs::MwcsGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsGraphType;
        TEMPLATE_GRAPH_TYPEDEFS(Graph);

        typedef typename std::vector<std::vector<std::vector<double>>> Table;
        typedef std::set<Node> NodeSet;
        typedef typename NodeSet::const_iterator NodeSetIt;
        typedef std::vector<Node> NodeVector;


        ColorCodingRooted() {
        }

        virtual ~ColorCodingRooted() {
        }

    private:
        double best_weight;
        std::vector<std::string> best_graph;
        uint32_t basic_colors;
        int subset_start;
        std::vector<std::string> rootSet;

    public:
        const time_t ctt = time(0);
        int number_of_nodes;
        int k;


        void printNodes(MwcsGraphType parser, NodeSet nodeSet) {
            for (auto root : nodeSet) {
                std::cout << "Rootnode! " << root << "\n";
            }
            //for (NodeIt n(nodeSet); n != lemon::INVALID; ++n) {

            //    std::cout << parser.getLabel(n);
            //}

        }

        void print_graph(std::vector<double> nodes, std::vector<std::string> labels, std::vector<uint32_t> node_colors,
                         std::vector<std::vector<int>> neighbors) {
            for (int i = 0; i < nodes.size(); i++) {
                std::cout << labels[i] << ": " << "color: " << color_string(node_colors[i]) << " weight: " << nodes[i]
                          << " neigbors: ";
                for (int j = 0; j < neighbors[i].size(); j++) {
                    std::cout << labels[neighbors[i][j]] << " ";
                }
                std::cout << std::endl;
            }

        }

        std::string color_string(uint32_t src) {
            std::string result = "";
            for (int i = 1; i <= 32; i++) {
                if (1 << (i - 1) == (src & (1 << (i - 1)))) {
                    result += std::to_string(i) + " ";
                }
            }
            return result;
        }

        void print_path(std::vector<std::string> path) {
            for (auto label : path) {
                std::cout << label << " ";
            }
            std::cout << std::endl;
        }

        bool contains_roots(std::vector<std::string> path) {
            for (auto root : rootSet) {
                if (std::find(path.begin(), path.end(), root) == path.end()) {
                    return false;
                }
            }
            return true;
        }

        void print_table(Table table, std::vector<std::string> labels, std::vector<uint32_t> colorset) {
            std::cout << "----------------- START TABLE ---------------------" << std::endl;
            for (int i = 0; i < table.size(); i++) {
                for (int j = 0; j < table[i].size(); j++) {
                    std::cout << "Node " << labels[i] << " colors: " << color_string(colorset[j]);
                    if (table[i][j][0] == -DBL_MAX) {
                        std::cout << " weight: _ ";
                    } else {
                        std::cout << " weight: " << table[i][j][0];
                        std::vector<std::string> path;
                        for (int l = 1; l < table[i][j].size(); l++) {
                            path.push_back(labels[table[i][j][l]]);
                        }
                        std::cout << " with: ";
                        print_path(path);
                        if (table[i][j][0] > best_weight && contains_roots(path)) {
                            best_weight = table[i][j][0];
                            best_graph = path;
                        }
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << "----------------- END TABLE ---------------------" << std::endl;
            std::cout << "Max weight: " << best_weight << " with path ";
            print_path(best_graph);
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
                result->push_back(current);
                return;
            }
            if (start >= 32) return;

            current = current | (1 << (start - 1));
            get_subsets(size, get_next(start + 1, src), src, current, result);
            current = (current & ~(1 << (start - 1)));
            get_subsets(size, get_next(start + 1, src), src, current, result);
        }

        int get_next(int start, uint32_t src) {
            while (1 << (start - 1) != (src & (1 << (start - 1)))) {
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

        void create_colors(std::vector<u_int32_t> *node_colors, int nodes, int k, std::vector<std::string> labels) {
            int rootColor = k -rootSet.size() + 1;
            for (int i = 0; i < nodes; i++) {
                // if node is not a rootnode
                if (std::find(rootSet.begin(), rootSet.end(), labels[i]) == rootSet.end()) {
                    // use random colors: 1 to k - rootsetsize. Leaving last colors for roots
                    int random = (rand() % (k - rootSet.size())) + 1;
                    std::cout << "normal node: " << labels[i] << " color: " << random << std::endl;
                    uint32_t color = 0;
                    color = (color | (1 << (random - 1)));
                    node_colors->push_back(color);
                } else {
                    std::cout << "root node " << labels[i] << " color: " << rootColor << std::endl;
                    uint32_t rcolor = 0;
                    rcolor = (rcolor | (1 << (rootColor - 1)));
                    node_colors->push_back(rcolor);
                    rootColor += 1;
                }
            }

        }

        void init_table(Table *table, std::vector<double> nodes, std::vector<u_int32_t> node_colors) {

            for (int i = 0; i < number_of_nodes; i++) {
                std::vector<std::vector<double>> temp_colors;
                temp_colors.reserve(pow(2,k) - 1);
                for (int j = 1; j <= k; j++) {
                    std::vector<double> weight;

                    if(1 << (j - 1) == (node_colors[i] & (1 << (j - 1)))) {
                        weight.push_back(nodes[i]);
                        weight.push_back(i);
                    } else {
                        weight.push_back(-DBL_MAX);
                    }
                    temp_colors.push_back(weight);
                }
                table->push_back(temp_colors);
            }
        }

        void init_vertices(MwcsGraphType &parser, std::vector<double> *nodes, std::vector<std::string> *labels,
                           std::vector<std::vector<int>> *neighbors, NodeSet rootNodeSet) {

            const Graph &g = parser.getGraph();

            for (auto rootnode : rootNodeSet) {
                rootSet.push_back(parser.getLabel(rootnode));
                //nodes->push_back(parser.getScore(rootnode));
                //labels->push_back(parser.getLabel(rootnode));
            }


            for (NodeIt n(g); n != lemon::INVALID; ++n) {
                nodes->push_back(parser.getScore(n));
                labels->push_back(parser.getLabel(n));
            }
            number_of_nodes = nodes->size();

            for (NodeIt n(g); n != lemon::INVALID; ++n) {
                std::vector<int> temp;
                for (IncEdgeIt edgeIt(g, n); edgeIt != lemon::INVALID; ++edgeIt) {
                    std::string nlabel = parser.getLabel(g.oppositeNode(n, edgeIt));
                    for (int j = 0; j < labels->size(); j++) {
                        if (nlabel.compare(labels->at(j)) == 0) {
                            temp.push_back(j);
                        }
                    }
                }
                neighbors->push_back(temp);
            }
        }

        void init(MwcsPreGraphType &parser, int number_of_colors, int number_of_iterations, NodeSet rootNodeSet) {
            std::cout << "ROOTED VERSION OF COLOR CODING \n";
            best_weight = -DBL_MAX;
            //best_graph = "";
            k = subset_start = number_of_colors;
            basic_colors = 0;


            // preprocess graph
            //parser.preprocess(NodeSet());
            //const Graph &g = parser.getGraph();

            // init datastructure
            std::vector<u_int32_t> colorset;
            std::vector<double> nodes;
            std::vector<std::string> labels;
            std::vector<std::vector<int>> neighbors;

            colorset.reserve(pow(2,k) - 1);
            nodes.reserve(parser.getNodeCount());
            labels.reserve(parser.getNodeCount());
            neighbors.reserve(parser.getNodeCount());

            init_colorset(&colorset);
            init_vertices(parser, &nodes, &labels, &neighbors, rootNodeSet);

            // iterations
            for (int iter = 0; iter < number_of_iterations; iter++) {
                std::cout << "Iteration: " << iter << " succeeded!" << std::endl;

                int nr_of_subsets = 0;
                subset_start = k;

                // setup data structure
                std::vector<uint32_t> node_colors;
                node_colors.reserve(parser.getNodeCount());
                Table table;
                table.reserve(parser.getNodeCount());

                create_colors(&node_colors, nodes.size(), number_of_colors, labels);
                init_table(&table, nodes, node_colors);

                // for each, color i among k colors do
                for (int i = 2; i <= k; i++) {

                    // create all possible sub color sets of size i
                    subset_start += nr_of_subsets;
                    nr_of_subsets = binomial_coeff(k, i);

                    // for each vertex u in V do
                    for (int current_node = 0; current_node < nodes.size(); current_node++) {
                        // for each colorset T of size i do
                        for (int t = subset_start; t < (subset_start + nr_of_subsets); t++) {
                            std::vector<double> best;
                            best.push_back(-DBL_MAX);


                            // TEST CODE
                            // is kleur van v in t?

                            if (1 << (node_colors[current_node]) != (colorset[t] & (node_colors[current_node]))) {
                                // TOT HIER

                                for (int current_neighbor = 0;
                                     current_neighbor < neighbors[current_node].size(); current_neighbor++) {


                                    // TEST CODE 2
                                    // is kleur van neighbor u in t-v?
                                    if (1 << (node_colors[neighbors[current_node][current_neighbor]]) !=
                                            (colorset[t] & (node_colors[neighbors[current_node][current_neighbor]]))
                                        && node_colors[current_node] != node_colors[current_neighbor]) {
                                        // TOT HIER


                                        //for each colorset Q s.t. Q is subset of T AND Q != empty
                                        std::vector<uint32_t> q_subsets_t;
                                        create_subsets(colorset[t], t, &q_subsets_t);
                                        for (int q = 1; q < q_subsets_t.size(); q++) {
                                            // R = T / Q
                                            uint32_t r_subset_t = colorset[t] & (~q_subsets_t[q]);

                                            // is kleur van v element van subset?
                                            if (1 << (node_colors[current_node]) !=
                                                (q_subsets_t[q] & (node_colors[current_node]))) {

                                                // ja -> is kleur van neighbor element van r=t-q?
                                                if (1 << (node_colors[neighbors[current_node][current_neighbor]] !=
                                                          (r_subset_t &
                                                           (node_colors[neighbors[current_node][current_neighbor]])))) {

                                                    std::vector<double> candidate;
                                                    candidate.push_back(-DBL_MAX);
                                                    std::vector<double> first;
                                                    std::vector<double> second;
                                                    for (int index = 0; index < table[current_node].size(); index++) {
                                                        if (colorset[index] == q_subsets_t[q]) {
                                                            candidate[0] = table[current_node][index][0];
                                                            first = table[current_node][index];
                                                        }
                                                    }
                                                    for (int indexn = 0; indexn <
                                                                         table[neighbors[current_node][current_neighbor]].size(); indexn++) {
                                                        if (colorset[indexn] == r_subset_t) {
                                                            candidate[0] += table[neighbors[current_node][current_neighbor]][indexn][0];
                                                            second = table[neighbors[current_node][current_neighbor]][indexn];
                                                        }
                                                    }
                                                    if (candidate[0] > best[0]) {
                                                        best = candidate;
                                                        for (int ifi = 1; ifi < first.size(); ifi++) {
                                                            best.push_back(first[ifi]);
                                                        }
                                                        for (int is = 1; is < second.size(); is++) {
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

                            }
                            table[current_node].push_back(best);
                        }
                    }
                }
                //print_graph(nodes, labels, node_colors, neighbors);
                print_table(table, labels, colorset);
            }

        }

    };
}
#endif //HEINZ_COLORCODINGROOTED_H
