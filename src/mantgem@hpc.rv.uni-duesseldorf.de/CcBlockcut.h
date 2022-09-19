//
// Created by ishuena on 26.11.18.
//

#ifndef HEINZ_CCBLOCKCUT_H
#define HEINZ_CCBLOCKCUT_H

#include <mwcspreprocessedgraph.h>
#include "solver/blockcuttree.h"
#include "CcBlockcut.h"
#include "CcUnrooted.h"
#include "CcRooted.h"
#include <ctime>
#include <algorithm>
#include <vector>
#include <cfloat>

namespace colcod {
    template<typename GR,
            typename NWGHT = typename GR::template NodeMap<double>,
            typename NLBL = typename GR::template NodeMap<std::string>,
            typename EWGHT = typename GR::template EdgeMap<double>>
    class CcBlockcut {
    public:
        typedef GR Graph;
        typedef NWGHT WeightNodeMap;
        typedef NLBL LabelNodeMap;
        typedef EWGHT WeightEdgeMap;

        typedef nina::mwcs::MwcsPreprocessedGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsPreGraphType;
        typedef nina::mwcs::MwcsGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsGraphType;
        TEMPLATE_GRAPH_TYPEDEFS(Graph);

        typedef std::set<Node> NodeSet;
        typedef typename std::pair<std::string, double> CcResult; //declaration

        CcBlockcut() {
        }

        virtual ~CcBlockcut() {
        }

    protected:
        typedef lemon::ListBpGraph Tree;
        typedef nina::BlockCutTree<Graph> BlockCutTreeType;
        typedef typename BlockCutTreeType::Tree BcTree;
        typedef typename BlockCutTreeType::BlockNodeIt BcTreeBlockNodeIt;
        typedef Tree::BlueNode BlockNode;
        typedef std::set<BlockNode> BlockNodeSet;
        typedef typename BlockCutTreeType::BlockNodeSet BcTreeBlockNodeSet;
        typedef typename BlockCutTreeType::BlockNodeSetIt BcTreeBlockNodeSetIt;
        typedef typename BlockCutTreeType::BoolTreeBlockNodeMap BcTreeBoolBlockNodeMap;
        typedef typename BlockCutTreeType::BlockNode BcTreeBlockNode;
        typedef typename BlockCutTreeType::CutNode BcTreeCutNode;
        typedef typename BlockCutTreeType::CutNodeIt BcTreeCutNodeIt;
        typedef typename BlockCutTreeType::TreeEdge BcTreeEdge;
        typedef typename BlockCutTreeType::TreeIncEdgeIt BcTreeIncEdgeIt;
    public:

        //return blockcut->run(parser, nodes, labels, neighbors, nr_of_colors, nr_of_iterations);

        void update_results(CcResult rootedResult, CcResult unrootedResult, CcResult &bestResult) {
            if (rootedResult.second >= unrootedResult.second && rootedResult.second >= bestResult.second) {
                bestResult.first = rootedResult.first;
                bestResult.second = rootedResult.second;
            } else if (unrootedResult.second > bestResult.second) {
                bestResult.first = unrootedResult.first;
                bestResult.second = unrootedResult.second;
            }
        }

        void update_results(CcResult unrootedResult, CcResult &bestResult) {
            if (unrootedResult.second > bestResult.second) {
                bestResult.first = unrootedResult.first;
                bestResult.second = unrootedResult.second;
            }
        }

        void init_vertices(MwcsPreGraphType &parser, std::vector<double> *nodes, std::vector<std::string> *labels,
                           std::vector<std::vector<int>> *neighbors, NodeSet nodesInBlock) {
            nodes->clear();
            labels->clear();
            neighbors->clear();

            for (auto n : nodesInBlock) {
                labels->push_back(parser.getLabel(n));
                nodes->push_back(parser.getScore(n));
                //std::cout << "Node in block: " << parser.getLabel(n) << std::endl;
            }

            const Graph &g = parser.getGraph();
            for (auto n : nodesInBlock) {
                std::vector<int> temp;
                for (IncEdgeIt edgeIt(g, n); edgeIt != lemon::INVALID; ++edgeIt) {
                    std::string nlabel = parser.getLabel(g.oppositeNode(n, edgeIt));
                    if (std::find(labels->begin(), labels->end(), nlabel) != labels->end()) {
                        for (int j = 0; j < labels->size(); j++) {
                            if (nlabel.compare(labels->at(j)) == 0) {
                                temp.push_back(j);
                            }
                        }
                    }
                }
                neighbors->push_back(temp);
            }

        }

        CcResult run(MwcsPreGraphType &parser, int nr_of_iterations, int nr_of_colors) {
            std::vector<double> nodes;
            std::vector<std::string> labels;
            std::vector<std::vector<int>> neighbors;
            CcResult bestResult;
            CcRooted *rooted = new CcRooted();
            CcUnrooted *unrooted = new CcUnrooted();

            // preprocess graph
            parser.preprocess(NodeSet());
            //const Graph &g = parser.getGraph();

            //bestResult.first = "";
            //bestResult.second = -DBL_MAX;

            const Graph &g = parser.getGraph();
            int nComponents = parser.getComponentCount();

            BlockCutTreeType bcTree(g);
            bcTree.run();

            const BcTree &T = bcTree.getBlockCutTree();
            int nBlocks = bcTree.getNumBlockTreeNodes();
            std::vector<std::string> roots;
            std::cout << "Number of blocks: " << nBlocks << std::endl;
            if (nBlocks != 0) {
                for (int blockDegree = nBlocks > 1 ? 1 : 0; blockDegree >= 0; --blockDegree) {
                    const BcTreeBlockNodeSet &leaves = bcTree.getBlockNodeSetByDegree(blockDegree);
                    while (!leaves.empty()) {

                        BcTreeBlockNode b = *leaves.begin();

                        BcTreeEdge e(BcTreeIncEdgeIt(T, b));
                        BcTreeCutNode c = e != lemon::INVALID ? T.redNode(e) : lemon::INVALID;
                        Node orgC = c != lemon::INVALID ? bcTree.getArticulationPoint(c) : lemon::INVALID;

                        const NodeSet &nodesInBlock = bcTree.getRealNodes(b);

                        //debug print
                        std::cout << "Block size: " << nodesInBlock.size() << std::endl;

                        init_vertices(parser, &nodes, &labels, &neighbors, nodesInBlock);

                        if (orgC != lemon::INVALID) {
                            CcResult unrootedResult = unrooted->run(nodes, labels, neighbors, nr_of_colors,
                                                                    nr_of_iterations);
                            std::cout << "Cutnode: " << parser.getLabel(orgC) << std::endl;
                            roots.push_back(parser.getLabel(orgC));
                            CcResult rootedResult = rooted->run(nodes, labels, neighbors, nr_of_colors,
                                                                nr_of_iterations, roots);

                            parser.updateScore(orgC, rootedResult.second);
                            parser.updateLabel(orgC, rootedResult.first);
                            update_results(rootedResult, unrootedResult, bestResult);
                        } else {

                            CcResult unrootedResult = unrooted->run(nodes, labels, neighbors, nr_of_colors,
                                                                    nr_of_iterations);
                            update_results(unrootedResult, bestResult);
                        }

                        bcTree.removeBlockNode(b);
                        roots.clear();
                    }
                }
            }
            return bestResult;
        }
    };
}


#endif //HEINZ_CCBLOCKCUT_H
