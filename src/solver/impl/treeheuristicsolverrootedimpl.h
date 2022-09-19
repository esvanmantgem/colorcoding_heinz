/*
 * treeheuristicsolverrootedimpl.h
 *
 *  Created on: 11-jan-2013
 *      Author: M. El-Kebir
 */

#ifndef TREEHEURISTICSOLVERROOTEDIMPL_H
#define TREEHEURISTICSOLVERROOTEDIMPL_H

#include "solverrootedimpl.h"
#include "treeheuristicsolverimpl.h"
#include "treesolverrootedimpl.h"
#include <assert.h>
#include <lemon/kruskal.h>

namespace nina {
namespace mwcs {

template<typename GR,
         typename NWGHT = typename GR::template NodeMap<double>,
         typename NLBL = typename GR::template NodeMap<std::string>,
         typename EWGHT = typename GR::template EdgeMap<double> >
class TreeHeuristicSolverRootedImpl : public SolverRootedImpl<GR, NWGHT, NLBL, EWGHT>,
                                      public TreeHeuristicSolverImpl<GR, NWGHT, NLBL, EWGHT>
{
public:
  typedef GR Graph;
  typedef NWGHT WeightNodeMap;
  typedef NLBL LabelNodeMap;
  typedef EWGHT WeightEdgeMap;

  typedef SolverRootedImpl<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> Parent1;
  typedef TreeHeuristicSolverImpl<Graph, WeightNodeMap, LabelNodeMap, WeightEdgeMap> Parent2;

  typedef typename Parent1::MwcsGraphType MwcsGraphType;
  typedef typename Parent1::NodeSet NodeSet;
  typedef typename Parent1::NodeSetIt NodeSetIt;
  
  typedef typename Parent2::Options Options;
  typedef typename Parent2::EdgeHeuristic EdgeHeuristic;
  
  TEMPLATE_GRAPH_TYPEDEFS(Graph);

  using Parent1::_pMwcsGraph;
  using Parent1::_rootNodes;
  
  using Parent2::_options;
  using Parent2::_pFilterMap;
  using Parent2::_pSubG;
  using Parent2::_pMwcsSubGraph;
  using Parent2::_pEdgeCost;
  using Parent2::solveMonteCarlo;

protected:
  typedef typename Parent2::SubGraphType SubGraphType;
  typedef typename Parent2::MwcsSubGraphType MwcsSubGraphType;
  typedef typename Parent2::MwcsAnalyzeType MwcsAnalyzeType;
  typedef typename Parent2::SubBoolNodeMap SubBoolNodeMap;
  
  typedef TreeSolverRootedImpl<const SubGraphType, const WeightNodeMap, LabelNodeMap, DoubleEdgeMap> MwcsRootedSubTreeSolverType;
  
public:
  TreeHeuristicSolverRootedImpl(const Options& options)
    : Parent1()
    , Parent2(options)
    , _mwcsRootedSubTreeSolver()
  {
  }

  virtual void init(const MwcsGraphType& mwcsGraph,
                    const NodeSet& rootNodes)
  {
    Parent1::init(mwcsGraph, rootNodes);
    Parent2::init(mwcsGraph);
  }
  
  bool solve(double& score, double& solveUB, BoolNodeMap& solutionMap, NodeSet& solutionSet)
  {
    solveUB = std::numeric_limits<double>::max();
    return solveMonteCarlo(*_pMwcsGraph, _mwcsRootedSubTreeSolver, score, solutionMap, solutionSet);
  }
  
protected:
  void initSubTreeSolver()
  {
    _mwcsRootedSubTreeSolver.init(*_pMwcsSubGraph, _rootNodes);
  }
  
  bool solveSubTree(double& score, SubBoolNodeMap& solutionMap, NodeSet& solutionSet)
  {
    double scoreUB;
    return _mwcsRootedSubTreeSolver.solve(score, scoreUB, solutionMap, solutionSet);
  }
  
private:
  MwcsRootedSubTreeSolverType _mwcsRootedSubTreeSolver;
};

} // namespace mwcs
} // namespace nina

#endif // TREEHEURISTICSOLVERROOTEDIMPL_H
