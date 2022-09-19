/*
 * stppcstparser.h
 *
 *  Created on: 11-oct-2013
 *      Author: M. El-Kebir
 */

#ifndef STPPCSTPARSER_H
#define STPPCSTPARSER_H

#include <assert.h>
#include <string.h>
#include <map>
#include <string>
#include <limits>
#include <lemon/core.h>
#include <set>
#include "parser.h"
#include "utils.h"

namespace nina {
namespace mwcs {

template<typename GR>
class StpPcstParser : public Parser<GR>
{
public:
  /// Graph type
  typedef GR Graph;
  /// Base class type
  typedef Parser<GR> Parent;

private:
  TEMPLATE_GRAPH_TYPEDEFS(Graph);
  
public:
  /// Node set type
  typedef std::set<Node> NodeSet;

public:
  typedef typename Parent::InvIdNodeMap InvIdNodeMap;
  typedef typename Parent::IdNodeMap IdNodeMap;
  typedef typename Parent::WeightNodeMap WeightNodeMap;
  typedef typename Parent::WeightEdgeMap WeightEdgeMap;

  using Parent::safeGetline;
  using Parent::_filename;
  using Parent::_pG;
  using Parent::_pIdNodeMap;
  using Parent::_pInvIdNodeMap;
  using Parent::_pWeightNodeMap;
  using Parent::_pWeightEdgeMap;
  using Parent::_nNodes;
  using Parent::_nEdges;

private:
  bool parseHeader(std::istream& in, int& lineNumber);
  bool parseGraph(std::istream& in, int& lineNumber);
  bool parseNrNodes(std::istream& in, int& lineNumber);
  bool parseNrEdges(std::istream& in, int& lineNumber);
  bool parseNrTerminals(std::istream& in, int& lineNumber);
  bool parseEdge(std::istream& in, int& lineNumber);
  bool parseTerminal(std::istream& in, int& lineNumber);
  
public:
  StpPcstParser(const std::string& filename);
  bool parse();
  const std::string& getName() const
  {
    return _name;
  }
  
  NodeSet getRootNodes() const
  {
    return _rootNodes;
  }
  
  double getTotalNodeProfit() const
  {
    return _pV;
  }
  
protected:
  std::string _name;
  int _nOrgNodes;
  int _nOrgEdges;
  int _nTerminals;
  NodeSet _rootNodes;
  double _pV;
};

template<typename GR>
inline StpPcstParser<GR>::StpPcstParser(const std::string& filename)
  : Parent(filename)
  , _name()
  , _nOrgNodes(0)
  , _nOrgEdges(0)
  , _nTerminals(0)
  , _rootNodes()
{
}

template<typename GR>
inline bool StpPcstParser<GR>::parseHeader(std::istream& in, int& lineNumber)
{
  std::string line;
  if (safeGetline(in, line))
  {
    lineNumber++;
    return line.substr(0, 8) == "33D32945";
  }
  else
  {
    return false;
  }
}

template<typename GR>
inline bool StpPcstParser<GR>::parseNrTerminals(std::istream& in, int& lineNumber)
{
  std::string line;

  if (safeGetline(in, line))
  {
    std::string text;
    std::stringstream ss(line);
    lineNumber++;
    ss >> text;

    if (text != "Terminals")
    {
      std::cerr << "Error at line " << lineNumber << ": expected 'Terminals'" << std::endl;
      return false;
    }

    _nTerminals = -1;
    ss >> _nTerminals;
    if (_nTerminals < 0)
    {
      std::cerr << "Error at line " << lineNumber << ": terminal count must be nonnegative" << std::endl;
      return false;
    }
    return true;
  }
  else
  {
    std::cerr << "Premature end-of-file; expected terminal count" << std::endl;
    return false;
  }
}

template<typename GR>
inline bool StpPcstParser<GR>::parseNrNodes(std::istream& in, int& lineNumber)
{
  std::string line;

  if (safeGetline(in, line))
  {
    std::string text;
    std::stringstream ss(line);
    lineNumber++;
    ss >> text;

    if (text != "Nodes")
    {
      std::cerr << "Error at line " << lineNumber << ": expected 'Nodes'" << std::endl;
      return false;
    }

    ss >> _nOrgNodes;
    return true;
  }
  else
  {
    std::cerr << "Premature end-of-file; expected node count" << std::endl;
    return false;
  }
}

template<typename GR>
inline bool StpPcstParser<GR>::parseNrEdges(std::istream& in, int& lineNumber)
{
  std::string line;

  if (safeGetline(in, line))
  {
    std::string text;
    std::stringstream ss(line);
    lineNumber++;
    ss >> text;

    if (text != "Edges")
    {
      std::cerr << "Error at line " << lineNumber << ": expected 'Edges'" << std::endl;
      return false;
    }

    ss >> _nOrgEdges;
    
    // because of the transformation every edge becomes a node of itself
    _nNodes = _nOrgNodes + _nOrgEdges;
    // duplicate edges because of transformation
    _nEdges = 2 * _nOrgEdges;

    _pG->reserveNode(_nNodes);
    _pG->reserveEdge(_nEdges);
    
    return true;
  }
  else
  {
    std::cerr << "Premature end-of-file; expected edge count" << std::endl;
    return false;
  }
}

template<typename GR>
inline bool StpPcstParser<GR>::parseGraph(std::istream& in, int& lineNumber)
{
  std::string line;
  char buf[1024];
  
  // skip until "Name"
  while (safeGetline(in, line) && line.substr(0, 4) != "Name")
    lineNumber++;
  
  if (line.size() < 6)
  {
    std::cerr << "Error: missing 'Name'" << std::endl;
    return false;
  }
  _name = line.substr(5);

  // skip until "SECTION Graph"
  while (safeGetline(in, line) && line != "SECTION Graph")
    lineNumber++;

  if (line != "SECTION Graph")
  {
    std::cerr << "Error: missing 'SECTION Graph'" << std::endl;
    return false;
  }

  if (!parseNrNodes(in, lineNumber) || !parseNrEdges(in, lineNumber))
  {
    return false;
  }

  // add nodes
  for (int i = 0; i < _nOrgNodes; i++)
  {
    Node u = _pG->addNode();
    _pWeightNodeMap->set(u, 0);
    
    snprintf(buf, 1024, "%d", _pG->id(u) + 1);
    _pIdNodeMap->set(u, buf);
    (*_pInvIdNodeMap)[buf] = u;
  }

  // add edges
  for (int i = 0;i < _nOrgEdges; i++)
  {
    if (!parseEdge(in, lineNumber))
    {
      return false;
    }
  }

  // skip until "SECTION Terminals"
  while (safeGetline(in, line) && line != "SECTION Terminals")
    lineNumber++;

  if (line != "SECTION Terminals")
  {
    std::cerr << "Error: missing 'SECTION Terminals'" << std::endl;
    return false;
  }

  if (!parseNrTerminals(in, lineNumber))
  {
    return false;
  }

  _pV = 0;
  for (int i = 0; i < _nTerminals; i++)
  {
    if (!parseTerminal(in, lineNumber))
    {
      return false;
    }
  }

  return true;
}

template<typename GR>
inline bool StpPcstParser<GR>::parseEdge(std::istream& in, int& lineNumber)
{
  std::string line;
  char buf[1024];

  if (safeGetline(in, line))
  {
    std::string text;
    std::stringstream ss(line);
    lineNumber++;
    ss >> text;

    if (text != "E")
    {
      std::cerr << "Error at line " << lineNumber << ": expected 'E'" << std::endl;
      return false;
    }

    int idU = -1, idV = -1;
    double costUV = std::numeric_limits<double>::max();
    ss >> idU >> idV >> costUV;

    if (!(0 < idU && idU <= _nNodes) || !(0 < idV && idV <= _nNodes))
    {
      std::cerr << "Error at line " << lineNumber << ": expected node id in [1, "
                << _nNodes << "]" << std::endl;
      return false;
    }
    
    if (costUV == std::numeric_limits<double>::max())
    {
      std::cerr << "Error at line " << lineNumber << ": expected real-valued edge cost"
                << std::endl;
      return false;
    }

    Node u = _pG->nodeFromId(idU - 1);
    Node v = _pG->nodeFromId(idV - 1);
    Node uv = _pG->addNode();
    _pG->addEdge(u, uv);
    _pG->addEdge(uv, v);
    
    _pWeightNodeMap->set(uv, -costUV);
    snprintf(buf, 1024, "%d--%d", idU, idV);
    _pIdNodeMap->set(uv, buf);
    (*_pInvIdNodeMap)[buf] = uv;

    return true;
  }
  else
  {
    std::cerr << "Premature end-of-file; expected edge" << std::endl;
    return false;
  }
}

template<typename GR>
inline bool StpPcstParser<GR>::parseTerminal(std::istream& in, int& lineNumber)
{
  std::string line;

  if (safeGetline(in, line))
  {
    std::string text;
    std::stringstream ss(line);
    lineNumber++;
    ss >> text;

    if (text != "TP" && text != "RootP")
    {
      std::cerr << "Error at line " << lineNumber << ": expected 'TP' or 'RootP'" << std::endl;
      return false;
    }

    int idU = -1;
    double weightU = -std::numeric_limits<double>::max();

    ss >> idU >> weightU;
    
    if (text == "RootP")
    {
      Node u = _pG->nodeFromId(idU - 1);
      _rootNodes.insert(u);
    }
    else
    {
      if (!(0 < idU && idU <= _nNodes))
      {
        std::cerr << "Error at line " << lineNumber << ": expected node id in [1, "
                  << _nNodes << "]" << std::endl;
        return false;
      }

      if (weightU == -std::numeric_limits<double>::max())
      {
        std::cerr << "Error at line " << lineNumber << ": expected real-valued node weight"
                  << std::endl;
        return false;
      }
      
      Node u = _pG->nodeFromId(idU - 1);
      _pWeightNodeMap->set(u, weightU);
      _pV += weightU;
    }

    return true;
  }
  else
  {
    std::cerr << "Premature end-of-file; expected terminal" << std::endl;
    return false;
  }
}

template<typename GR>
inline bool StpPcstParser<GR>::parse()
{
  if (!_pG)
    return false;

  std::ifstream in(_filename.c_str());
  if (!in.good())
  {
    std::cerr << "Error: could not open file "
              << _filename << " for reading" << std::endl;
    return false;
  }

  _pG->clear();

  int lineNumber = 0;
  return parseHeader(in, lineNumber) && parseGraph(in, lineNumber);
}

} // namespace mwcs
} // namespace nina

#endif // STPPCSTPARSER_H
