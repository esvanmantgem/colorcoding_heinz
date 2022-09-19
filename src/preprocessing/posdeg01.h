/*
 * posdeg01.h
 *
 *  Created on: 8-mar-2014
 *      Author: M. El-Kebir
 */

#ifndef POSDEG01_H
#define POSDEG01_H

#include <lemon/core.h>
#include <string>
#include <vector>
#include <set>
#include "rule.h"

namespace nina {
namespace mwcs {

template<typename GR,
         typename WGHT = typename GR::template NodeMap<double> >
class PosDeg01 : public Rule<GR, WGHT>
{
public:
  typedef GR Graph;
  typedef WGHT WeightNodeMap;
  typedef Rule<GR, WGHT> Parent;
  typedef typename Parent::NodeMap NodeMap;
  typedef typename Parent::NodeSet NodeSet;
  typedef typename Parent::NodeSetIt NodeSetIt;
  typedef typename Parent::NodeSetMap NodeSetMap;
  typedef typename Parent::DegreeNodeMap DegreeNodeMap;
  typedef typename Parent::DegreeNodeSetVector DegreeNodeSetVector;
  typedef typename Parent::LabelNodeMap LabelNodeMap;

  TEMPLATE_GRAPH_TYPEDEFS(Graph);

  using Parent::remove;
  using Parent::merge;
  using Parent::extract;

  PosDeg01();
  virtual ~PosDeg01() {}
  virtual int apply(Graph& g,
                    const NodeSet& rootNodes,
                    LabelNodeMap& label,
                    WeightNodeMap& score,
                    NodeSetMap& mapToPre,
                    NodeSetMap& preOrigNodes,
                    NodeSetMap& neighbors,
                    int& nNodes,
                    int& nArcs,
                    int& nEdges,
                    DegreeNodeMap& degree,
                    DegreeNodeSetVector& degreeVector,
                    double& LB);

  virtual std::string name() const { return "PosDeg01"; }
};

template<typename GR, typename WGHT>
inline PosDeg01<GR, WGHT>::PosDeg01()
  : Parent()
{
}

template<typename GR, typename WGHT>
inline int PosDeg01<GR, WGHT>::apply(Graph& g,
                                     const NodeSet& rootNodes,
                                     LabelNodeMap& label,
                                     WeightNodeMap& score,
                                     NodeSetMap& mapToPre,
                                     NodeSetMap& preOrigNodes,
                                     NodeSetMap& neighbors,
                                     int& nNodes,
                                     int& nArcs,
                                     int& nEdges,
                                     DegreeNodeMap& degree,
                                     DegreeNodeSetVector& degreeVector,
                                     double& LB)
{
  if (degreeVector.size() == 0)
  {
    // nothing to remove, there are no degree 0 nodes
    return 0;
  }
  
  // positive deg 0 nodes smaller than LB are to be removed
  const NodeSet& nodes0 = degreeVector[0];
  for (NodeSetIt nodeIt = nodes0.begin(); nodeIt != nodes0.end(); ++nodeIt)
  {
    Node v = *nodeIt;
    if (0 <= score[v] && score[v] < LB && rootNodes.find(v) == rootNodes.end())
    {
      assert(IncEdgeIt(g, v) == lemon::INVALID);
      remove(g, mapToPre, preOrigNodes, neighbors,
             nNodes, nArcs, nEdges,
             degree, degreeVector, v);
      return 1;
    }
  }
  
  if (degreeVector.size() <= 1)
  {
    // nothing to remove, there are no degree 1 nodes
    return 0;
  }
  
  const NodeSet& nodes1 = degreeVector[1];
  for (NodeSetIt nodeIt = nodes1.begin(); nodeIt != nodes1.end(); ++nodeIt)
  {
    Node v = *nodeIt;
    
    if (score[v] >= 0 && rootNodes.find(v) == rootNodes.end())
    {
      if (score[v] >= LB && rootNodes.empty())
      {
        extract(g, label, score,
                mapToPre, preOrigNodes, neighbors,
                nNodes, nArcs, nEdges,
                degree, degreeVector, v);
      }
        
      Node u = g.oppositeNode(v, IncEdgeIt(g, v));
      
      // u may be a root node, that's why we should keep it!
      merge(g, label, score,
            mapToPre, preOrigNodes, neighbors,
            nNodes, nArcs, nEdges,
            degree, degreeVector, v, u, LB);

      return 1;
    }
  }

  return 0;
}

} // namespace mwcs
} // namespace nina

#endif // POSDEG01_H
