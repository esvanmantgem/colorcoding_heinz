//
// Created by ishuena on 06.10.17.
//

#ifndef HEINZ_COLCOD_H
#define HEINZ_COLCOD_H

#include <boost/functional/hash.hpp>
#include <mwcspreprocessedgraph.h>
#include "colorcoding/Vertex.h"
#include "colorcoding/Vertices.h"
#include <ctime>
#include <algorithm>

namespace std
{
    using VertexColorPair = pair<colcod::Vertex, colcod::ColorSet>;
    template<>
    struct hash<VertexColorPair>
    {
        size_t operator()(VertexColorPair const& p) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, p.first.get_name());
            boost::hash_combine(seed, p.second.get_colors());

            return seed;
        }
    };
}


namespace colcod {
    template<typename GR,
            typename NWGHT = typename GR::template NodeMap<double>,
            typename NLBL = typename GR::template NodeMap<std::string>,
            typename EWGHT = typename GR::template EdgeMap<double>>
    class ColorCoding {

    public:
        typedef GR Graph;
        typedef NWGHT WeightNodeMap;
        typedef NLBL LabelNodeMap;
        typedef EWGHT WeightEdgeMap;

        typedef nina::mwcs::MwcsPreprocessedGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsPreGraphType;
        typedef nina::mwcs::MwcsGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsGraphType;
        TEMPLATE_GRAPH_TYPEDEFS(Graph);

        typedef typename std::unordered_map<std::VertexColorPair, colcod::Vertices> Table;
        typedef std::set<Node> NodeSet;
        typedef typename NodeSet::const_iterator NodeSetIt;
        typedef typename NodeSet::iterator NodeSetNonConstIt;
        typedef std::vector<Node> NodeVector;
        typedef typename NodeVector::const_iterator NodeVectorIt;
        typedef typename NodeVector::iterator NodeVectorNonConstIt;


        ColorCoding()
        //   : _score(0), _scoreUB(std::numeric_limits<double>::max()), _pSolutionMap(NULL), _solutionSet() {}
        {
        }
        virtual ~ColorCoding() {
            // delete _pSolutionMap;
        }

    private:
        int best_weight;
        std::string best_graph;


    protected:
        // double _score;
        // double _scoreUB;
        //  BoolNodeMap *_pSolutionMap;
        //  NodeSet _solutionSet;


    public:
        const time_t ctt = time(0);


        void print_table(Table table) {
            //std::cout << asctime(localtime(&ctt)) << std::endl;
            //void print_table(std::unordered_map<std::VertexColorPair, colcod::Vertices> table) {
            std::cout << "----------------- START TABLE ---------------------" << std::endl;
            for (auto it : table) {
                std::VertexColorPair current = it.first;
                colcod::Vertex v = it.first.first;
                colcod::ColorSet c = it.first.second;
                std::cout << "Vertex: " << v.get_name() << " colors: " << c.string() << " weight: " << it.second.get_weight() << " graph: " << it.second.string()<< std::endl;
                if (it.second.get_weight() > best_weight) {
                    best_weight = it.second.get_weight();
                    best_graph = it.second.string();
                }
            }
            std::cout << "----------------- END TABLE ---------------------" << std::endl;
        }

        void init_table(std::unordered_map<std::VertexColorPair, colcod::Vertices> *table, colcod::Vertices vertices) {
            for (int i = 0; i < vertices.size(); i++) {
                colcod::Vertex current = vertices.get_vertex(i);
                colcod::ColorSet c;
                c.add(current.get_color());
                std::VertexColorPair tkey = {current, c};
                colcod::Vertices v;
                v.add_vertex(current);
                /* Check which method is better */
                //table->emplace(tkey, current.get_weight());
                std::pair<std::VertexColorPair, colcod::Vertices> entry(tkey, v);
                table->insert(entry);
            }
        }

        Vertices init_vertices(MwcsGraphType& parser, int number_of_colors) {

            const Graph &g = parser.getGraph();

            colcod::Vertices vertices;

            for (NodeIt n(g); n != lemon::INVALID; ++n) {
                std::string label = parser.getLabel(n);
                double weight = parser.getScore(n);
                //colcod::Vertex *vertex = new Vertex(pParser.getLabel(n), pParser.getScore(n));
                int color = (rand() % number_of_colors) + 1;
                colcod::Vertex current(label, weight, color);

                for (IncEdgeIt edgeIt(g, n); edgeIt !=lemon::INVALID; ++edgeIt) {
                    std::string nlabel = parser.getLabel(g.oppositeNode(n, edgeIt));
                    double nweight = parser.getScore(g.oppositeNode(n, edgeIt));
                    colcod::Vertex neighbor(nlabel, nweight);
                    current.set_neighbor(neighbor);
                }
                vertices.add_vertex(current);
            }
            return vertices;
        }

        //std::pair<std::VertexColorPair, colcod::Vertices> find_best_path(std::unordered_map<std::VertexColorPair, colcod::Vertices> table) {
        //    for (auto it : table) {

        //    }
        //}

        std::unordered_map<std::VertexColorPair, colcod::Vertices> find_max(std::unordered_map<std::VertexColorPair, colcod::Vertices> table, colcod::Vertex vertex, colcod::Vertex neighbor, colcod::ColorSet set) {
            colcod::Vertices result;
            std::vector<colcod::ColorSet> powerset = set.create_powerset();
            std::VertexColorPair best_pair = {vertex, vertex.get_color()};

            bool new_best = false;
            for (auto it : powerset) {
                //std::cout << "find_max: powerset: " << it.string() << std::endl;
                //std::cout << asctime(localtime(&ctt)) << std::endl;

                std::unordered_map<std::VertexColorPair, colcod::Vertices>::const_iterator vertex_pair = table.find({vertex, it});
                // als subset van powerset staat in table met vertex
                if (vertex_pair != table.end()) {
                    // als complement subset van powerset staat in table met neighbor
                    colcod::ColorSet temp = set.copy();
                    temp.remove(it);
                    std::unordered_map<std::VertexColorPair, colcod::Vertices>::const_iterator neighbor_pair = table.find({neighbor, temp});
                    if (neighbor_pair != table.end()) {
                        //if (!neighbor_pair->second.has_vertex(vertex)) {
                        if (std::find(neighbor_pair->second.vertices.begin(), neighbor_pair->second.vertices.end(), vertex) == neighbor_pair->second.vertices.end()) {
                            int candidate = vertex_pair->second.get_weight() + neighbor_pair->second.get_weight();
                            if (candidate > result.get_weight()) {
                                result.add_vertex(vertex);
                                result.add_vertex(neighbor);
                                for (int i = 0; i < neighbor_pair->second.size(); i ++) {
                                    //std::cout << "neigbor-pair: " << neighbor_pair->second.string() << std::endl;
                                    //std::cout << asctime(localtime(&ctt)) << std::endl;
                                    result.add_vertex(neighbor_pair->second.get_vertex(i));
                                }
                                new_best = true;
                                best_pair = {vertex, set};
                            }
                        }
                    }
                }
            }
            std::pair<std::VertexColorPair, colcod::Vertices> entry (best_pair, result);
            if (new_best) {

                std::unordered_map<std::VertexColorPair, colcod::Vertices>::const_iterator current_pair = table.find(
                        {best_pair.first, best_pair.second});
                if (current_pair != table.end()) {
                    if (current_pair->second.get_weight() < result.get_weight()) table[best_pair] = result;
                } else {
                    table.insert(entry);
                }
            }
            return table;
        }


        void init(MwcsPreGraphType& parser, int number_of_colors, int number_of_iterations) {
            best_weight = INT_MIN;
            best_graph = "";
            colcod::ColorSet colors(number_of_colors);
            std::cout << number_of_colors << std::endl;

            parser.preprocess(NodeSet());


            for (int l = 0; l < number_of_iterations; l++) {
                //std::cout << "For iteration nr: " << l << std::endl;
                std::unordered_map<std::VertexColorPair, colcod::Vertices> table;
                Vertices vertices = init_vertices(parser, number_of_colors);
                init_table(&table, vertices);

                for (int i = 2; i <= number_of_colors; i++) {
                    //std::cout << "For color nr: " << i << std::endl;
                    // create all possible sub color sets of size i
                    std::vector<colcod::ColorSet> subsets = colors.create_subsets(i);

                    //pDEBUG print subsetgs
                    //for (auto iter : subsets) {
                    //    std::cout << "subsets: " << iter.string() << std::endl;
                    //}
                    for (int j = 0; j < vertices.size(); j++) {
                        //std::cout << "For vertex: " << vertices.get_vertex(j).get_name() << std::endl;
                        colcod::Vertex current = vertices.get_vertex(j);

                        for (int k = 0; k < current.get_number_of_neighbors(); k++) {
                            colcod::Vertex neighbor = vertices.get_vertex(current.get_neighbor(k).get_name());
                            //std::cout << "For neighbor: " << neighbor.get_name() << std::endl;
                            //std::cout << asctime(localtime(&ctt)) << std::endl;

                            // for all subsets_q in t
                            for (auto it : subsets) {

                                // is kleur van v element van subset?
                                if (it.has_color(current.get_color())) {
                                    //std::cout << "The current colorset: " << it.string() << " has color " << current.get_color() << std::endl;
                                    colcod::ColorSet complement = it.copy();
                                    complement.remove(current.get_color());

                                    // ja -> is kleur van neighbor element van t-q?
                                    if (complement.has_color(neighbor.get_color())) {
                                        //std::cout << "Neigbor color is element of t-q, so find max in table" << std::endl;
                                        table = find_max(table, current, neighbor, it);
                                    }
                                }
                            }
                        }
                    }
                }
                print_table(table);
            }
            std::cout << "Best graph: " << best_graph << " with weight: " << best_weight << std::endl;
        }

    };
}
#endif //HEINZ_COLCOD_H
