//
// Created by ishuena on 02.10.18.
//

#ifndef HEINZ_CCADAPTER_H
#define HEINZ_CCADAPTER_H

#include <mwcspreprocessedgraph.h>
#include <ctime>
#include <algorithm>
#include <vector>
#include <cfloat>
#include "CcUnrooted.h"
#include "CcRooted.h"
#include "solver/blockcuttree.h"
#include "CcBlockcut.h"


namespace colcod {

    template<typename GR,
            typename NWGHT = typename GR::template NodeMap<double>,
            typename NLBL = typename GR::template NodeMap<std::string>,
            typename EWGHT = typename GR::template EdgeMap<double>>
    class CcAdapter {

    public:
        typedef GR Graph;
        typedef NWGHT WeightNodeMap;
        typedef NLBL LabelNodeMap;
        typedef EWGHT WeightEdgeMap;
        typedef nina::mwcs::MwcsPreprocessedGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsPreGraphType;
        typedef nina::mwcs::MwcsGraph<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> MwcsGraphType;
        TEMPLATE_GRAPH_TYPEDEFS(Graph);
        typedef std::set<Node> NodeSet;

        typedef typename Graph::template NodeMap<Node> NodeMap;
        typedef typename std::pair<std::string, double> CcResult; //declaration

        CcAdapter() {
        }

        virtual ~CcAdapter() {
        }


    private:
    protected:
        typedef lemon::ListBpGraph Tree;
        typedef nina::BlockCutTree<Graph> BlockCutTreeType;
        typedef typename BlockCutTreeType::Tree BcTree;
        typedef typename BlockCutTreeType::BlockNodeSet BcTreeBlockNodeSet;
        typedef typename BlockCutTreeType::BlockNode BcTreeBlockNode;
        typedef typename BlockCutTreeType::CutNode BcTreeCutNode;
        typedef typename BlockCutTreeType::TreeEdge BcTreeEdge;
        typedef typename BlockCutTreeType::TreeIncEdgeIt BcTreeIncEdgeIt;
        typedef lemon::FilterNodes<const Graph, BoolNodeMap> SubGraph;
        typedef typename BlockCutTreeType::BoolTreeBlockNodeMap BcTreeBoolBlockNodeMap;
        typedef typename BlockCutTreeType::BlockNodeIt BcTreeBlockNodeIt;

        typedef std::vector<Edge> EdgeVector;
        typedef typename EdgeVector::const_iterator EdgeVectorIt;

    public:

        void initLocalGraph(const Graph& g,
                            const WeightNodeMap& scoreG,
                            const LabelNodeMap& labelG,
                            BoolNodeMap& filterG,
                            Graph& subG,
                            DoubleNodeMap& weightSubG,
                            LabelNodeMap& labelSubG,
                            NodeMap& mapToG,
                            MwcsPreGraphType& mwcsSubGraph)
        {
            SubGraph subTmpSameCompG(g, filterG);
            lemon::graphCopy(subTmpSameCompG, subG)
                    .nodeMap(scoreG, weightSubG)
                    .nodeMap(labelG, labelSubG)
                    .nodeCrossRef(mapToG)
                    .run();

            mwcsSubGraph.init(&subG, &labelSubG, &weightSubG, NULL);
        }

        void update_results(CcResult rootedResult, CcResult unrootedResult, CcResult &bestResult) {
            if (rootedResult.second >= unrootedResult.second && rootedResult.second >= bestResult.second) {
                bestResult.first = rootedResult.first;
                bestResult.second = rootedResult.second;
            } else if (unrootedResult.second > bestResult.second) {
                bestResult.first = unrootedResult.first;
                bestResult.second = unrootedResult.second;
            }
            std::cout << "Temp best: " << bestResult.first << " with " << bestResult.second << std::endl;
        }

        void update_results(CcResult unrootedResult, CcResult &bestResult) {
            if (unrootedResult.second > bestResult.second) {
                bestResult.first = unrootedResult.first;
                bestResult.second = unrootedResult.second;
            }
            std::cout << "Temp best: " << bestResult.first << " with " << bestResult.second << std::endl;
        }

        //void init_vertices(MwcsGraphType &parser, std::vector<double> *nodes, std::vector<std::string> *labels, std::vector<std::vector<int>> *neighbors) {
        void init_vertices(MwcsPreGraphType &parser, std::vector<double> *nodes, std::vector<std::string> *labels, std::vector<std::vector<int>> *neighbors) {
            const Graph &g = parser.getGraph();

            for (NodeIt n(g); n != lemon::INVALID; ++n) {
                nodes->push_back(parser.getScore(n));
                labels->push_back(parser.getLabel(n));
            }

            for (NodeIt n(g); n != lemon::INVALID; ++n) {
                std::vector<int> temp;
                for (IncEdgeIt edgeIt(g, n); edgeIt != lemon::INVALID; ++edgeIt) {
                    std::string nlabel = parser.getLabel(g.oppositeNode(n, edgeIt));
                    for (int j = 0; (unsigned) j < labels->size(); j++) {
                        if (nlabel.compare(labels->at(j)) == 0) {
                            temp.push_back(j);
                        }
                    }
                }
                neighbors->push_back(temp);
            }
        }

        //void init_vertices(MwcsGraphType &parser, std::vector<double> *nodes, std::vector<std::string> *labels, std::vector<std::vector<int>> *neighbors, NodeSet nodesInBlock) {
        void init_vertices(MwcsPreGraphType &parser, std::vector<double> *nodes, std::vector<std::string> *labels, std::vector<std::vector<int>> *neighbors, NodeSet nodesInBlock) {
            nodes->clear();
            labels->clear();
            neighbors->clear();

            for (auto n : nodesInBlock) {
                labels->push_back(parser.getLabel(n));
                nodes->push_back(parser.getScore(n));
            }

            const Graph &g = parser.getGraph();
            for (auto n : nodesInBlock) {
                std::vector<int> temp;
                for (IncEdgeIt edgeIt(g, n); edgeIt != lemon::INVALID; ++edgeIt) {
                    std::string nlabel = parser.getLabel(g.oppositeNode(n, edgeIt));
                    if (std::find(labels->begin(), labels->end(), nlabel) != labels->end()) {
                        for (int j = 0; (unsigned) j < labels->size(); j++) {
                            if (nlabel.compare(labels->at(j)) == 0) {
                                temp.push_back(j);
                            }
                        }
                    }
                }
                neighbors->push_back(temp);
            }
        }

        //std::vector<std::string> init_rootset(MwcsGraphType &parser, NodeSet rootNodeSet) {
        std::vector<std::string> init_rootset(MwcsPreGraphType &parser, NodeSet rootNodeSet) {
            std::vector<std::string> result;
            for (auto node : rootNodeSet) {
                result.push_back(parser.getLabel(node));
            }
            return result;
        }

        //inline void process_leaf(MwcsGraphType &parser, int nr_of_colors, int nr_of_iterations, Node orgC, CcResult &bestResult, NodeSet nodesInBlock) {
        inline void process_leaf(MwcsPreGraphType &parser, int nr_of_colors, int nr_of_iterations, Node orgC, CcResult &bestResult, NodeSet nodesInBlock) {
            std::vector<double> nodes;
            std::vector<std::string> labels;
            std::vector<std::vector<int>> neighbors;
            std::vector<std::string> roots;
            init_vertices(parser, &nodes, &labels, &neighbors, nodesInBlock);

            if (orgC != lemon::INVALID) {
                CcResult unrootedResult = call_unrooted(nodes, labels, neighbors, nr_of_colors, nr_of_iterations);
                std::cout << "Cutnode: " << parser.getLabel(orgC) << std::endl;
                roots.push_back(parser.getLabel(orgC));
                CcResult rootedResult = call_rooted(nodes, labels, neighbors, nr_of_colors, nr_of_iterations, roots);


                parser.updateScore(orgC, rootedResult.second);
                parser.updateLabel(orgC, rootedResult.first);
                update_results(rootedResult, unrootedResult, bestResult);
            } else {
                CcResult unrootedResult = call_unrooted(nodes, labels, neighbors, nr_of_colors, nr_of_iterations);
                update_results(unrootedResult, bestResult);
            }
        }

        //CcResult call_blockcut(MwcsGraphType &parser, int nr_of_colors, int nr_of_iterations) {
        CcResult call_blockcut(MwcsPreGraphType &parser, int nr_of_colors, int nr_of_iterations, bool noPreprocess) {
            CcResult bestResult;
            bestResult.first = "";
            bestResult.second = -DBL_MAX;

            //const Graph &g = parser.getGraph();
            //int nComponents = parser.getComponentCount();
            const Graph& g = parser.getGraph();
            BoolNodeMap allowedNodesSameComp(g);

            // 1. iterate over the components
            int nComponents = parser.getComponentCount();
            const IntNodeMap& comp = parser.getComponentMap();

            for (int compIdx = 0; compIdx < nComponents; ++compIdx) {
                lemon::mapFill(g, allowedNodesSameComp, false);
                for (NodeIt node(g); node != lemon::INVALID; ++node) {
                    allowedNodesSameComp[node] = comp[node] == compIdx;
                }

                Graph subG;
                DoubleNodeMap weightSubG(subG);
                LabelNodeMap labelSubG(subG);
                NodeMap mapToG(subG);
                MwcsPreGraphType mwcsSubGraph;

                // 2b. create subgraph
                initLocalGraph(g,
                               parser.getScores(),
                               parser.getLabels(),
                               allowedNodesSameComp,
                               subG,
                               weightSubG,
                               labelSubG,
                               mapToG,
                               mwcsSubGraph);

                if (!noPreprocess) {
                    mwcsSubGraph.preprocess(NodeSet());
                }
                const Graph& gsub = mwcsSubGraph.getGraph();
                const WeightNodeMap& score = mwcsSubGraph.getScores();

                /*
                        for (NodeIt n(gsub); n != lemon::INVALID; ++n) {
                            std::cout << "score: " << mwcsSubGraph.getScore(n);
                            std::cout << " weight: " << mwcsSubGraph.getLabel(n);
                            std::cout << std::endl;
                        }
*/
                    BlockCutTreeType bcTree(gsub);
                    bcTree.run();


                    // determine block negativity
                    const BcTree& T = bcTree.getBlockCutTree();
                /*
                    BcTreeBoolBlockNodeMap treeNeg(T, true);
                    for (BcTreeBlockNodeIt v(T); v != lemon::INVALID; ++v) {
                        const EdgeVector& realEdges = bcTree.getRealEdges(v);
                        for (EdgeVectorIt edgeIt = realEdges.begin();
                             edgeIt != realEdges.end(); ++edgeIt) {
                            treeNeg[v] = treeNeg[v] && (score[g.u(*edgeIt)] <= 0 && score[g.v(*edgeIt)] <= 0);
                        }
                    }
                    */
                    int nBlocks = bcTree.getNumBlockTreeNodes();

                    int blockIndex = 0;

                    std::cout << "Number of blocks: " << nBlocks << std::endl;
                    if (nBlocks != 0) {
                        for (int blockDegree = nBlocks > 1 ? 1 : 0; blockDegree >= 0; --blockDegree) {
                            const BcTreeBlockNodeSet &leaves = bcTree.getBlockNodeSetByDegree(blockDegree);
                            while (!leaves.empty()) {

                                BcTreeBlockNode b = *leaves.begin();

                                bool blockNegative = true;
                                const NodeSet &nodesInBlock = bcTree.getRealNodes(b);
                                for (auto n : nodesInBlock) {
                                    blockNegative = blockNegative && (parser.getScore(n) <= 0);
                                    std::cout << "current: " << blockNegative << " " << parser.getScore(n)  << std::endl;
                                    std::cout << "label: " << parser.getLabel(n) << std::endl;
                                }

                                if (!blockNegative) {
                                    BcTreeEdge e(BcTreeIncEdgeIt(T, b));
                                    BcTreeCutNode c = e != lemon::INVALID ? T.redNode(e) : lemon::INVALID;
                                    Node orgC = c != lemon::INVALID ? bcTree.getArticulationPoint(c) : lemon::INVALID;

                                    //const NodeSet &nodesInBlock = bcTree.getRealNodes(b);

                                    //debug print
                                    std::cout << "Block size: " << nodesInBlock.size() << std::endl;

                                    process_leaf(mwcsSubGraph, nr_of_colors, nr_of_iterations, orgC, bestResult,
                                                 nodesInBlock);
                                } else {
                                    std::cout << "Block negative, skipping block";
                                }
                                bcTree.removeBlockNode(b);
                            }
                        }
                    }
                //}
            }
            return bestResult;
        }

        CcResult call_unrooted(std::vector<double> nodes, std::vector<std::string> labels, std::vector<std::vector<int>> neighbors, int nr_of_colors, int nr_of_iterations) {
            CcUnrooted *unrooted = new CcUnrooted();
            return unrooted->run(nodes, labels, neighbors, nr_of_colors, nr_of_iterations);
        }

        CcResult call_rooted(std::vector<double> nodes, std::vector<std::string> labels, std::vector<std::vector<int>> neighbors,
                             int nr_of_colors, int nr_of_iterations, std::vector<std::string> roots) {

            CcRooted *rooted = new CcRooted();
            return rooted->run(nodes, labels, neighbors, nr_of_colors, nr_of_iterations, roots);
        }

        //void solve(MwcsGraphType &parser, std::string ccType, int nr_of_colors, int nr_of_iterations, NodeSet rootNodeSet) {
        void solve(MwcsPreGraphType &parser, std::string ccType, int nr_of_colors, int nr_of_iterations, NodeSet rootNodeSet, bool noPreprocess) {
            std::vector<double> nodes;
            std::vector<std::string> labels;
            std::vector<std::vector<int>> neighbors;

            //std::cout << "ff voor d build";
            // preprocess graph
            //if (!noPreprocess) parser.preprocess(NodeSet());
            //const Graph &g = parser.getGraph();
            if (ccType.compare("blockcut") == 0) {
                std::cout << "Color Coding BlockCut" << std::endl;

                CcResult result = call_blockcut(parser, nr_of_colors, nr_of_iterations, noPreprocess);
  //              std::cout << "FINAL RESULT: " << result.first << " WEIGHT " << result.second << std::endl;
std::cout << nr_of_colors << "; " << nr_of_iterations << "; " << result.first << "; " << result.second << "; " << std::endl;
            } else {
                if (!noPreprocess) parser.preprocess(NodeSet());
                init_vertices(parser, &nodes, &labels, &neighbors);

                if (ccType.compare("unrooted") == 0) {
                    std::cout << "Color Coding Unrooted" << std::endl;

                    CcResult result = call_unrooted(nodes, labels, neighbors, nr_of_colors, nr_of_iterations);
//                    std::cout << "FINAL RESULT: " << result.first << " WEIGHT " << result.second << std::endl;
std::cout << nr_of_colors << "; " << nr_of_iterations << "; " << result.first << "; " << result.second << "; " << std::endl;
                } else { //if (ccType.compare("rooted") == 0) {
                    std::cout << "Color Coding Rooted" << std::endl;

                    std::vector<std::string> roots = init_rootset(parser, rootNodeSet);
                    CcResult result = call_rooted(nodes, labels, neighbors, nr_of_colors, nr_of_iterations, roots);
                    //std::cout << "FINAL RESULT: " << result.first << " WEIGHT " << result.second << std::endl;
std::cout << nr_of_colors << "; " << nr_of_iterations << "; " << result.first << "; " << result.second << "; " << std::endl;
                }
            }
        }
    };
};


#endif //HEINZ_CCADAPTER_H
