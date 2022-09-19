/*
 *  mwcs-mc.cpp
 *
 *   Created on: 29-apr-2013
 *       Author: M. El-Kebir
 */

#include <iostream>
#include <lemon/arg_parser.h>
#include <lemon/time_measure.h>

#include "parser/mwcsparser.h"
#include "parser/stpparser.h"
#include "parser/stppcstparser.h"

#include "mwcsgraph.h"
#include "mwcsgraphparser.h"
#include "mwcspreprocessedgraph.h"

#include "solver/solver.h"
#include "solver/solverrooted.h"
#include "solver/solverunrooted.h"
#include "solver/impl/treeheuristicsolverrootedimpl.h"
#include "solver/impl/treeheuristicsolverunrootedimpl.h"

#include "mwcs.h"
#include "utils.h"
#include "config.h"

using namespace nina::mwcs;
using namespace nina;

typedef Parser<Graph> ParserType;
typedef MwcsParser<Graph> MwcsParserType;
typedef StpParser<Graph> StpParserType;
typedef StpPcstParser<Graph> StpPcstParserType;

typedef MwcsGraphParser<Graph> MwcsGraphType;
typedef MwcsPreprocessedGraph<Graph> MwcsPreprocessedGraphType;
typedef NegDeg01<Graph> NegDeg01Type;
typedef PosEdge<Graph> PosEdgeType;
typedef NegEdge<Graph> NegEdgeType;
typedef NegCircuit<Graph> NegCircuitType;
typedef NegDiamond<Graph> NegDiamondType;
typedef NegMirroredHubs<Graph> NegMirroredHubsType;
typedef NegDominatedHubs<Graph> NegDominatedHubsType;
typedef PosDeg01<Graph> PosDeg01Type;
typedef PosDiamond<Graph> PosDiamondType;

typedef Solver<Graph> SolverType;
typedef SolverRooted<Graph> SolverRootedType;
typedef SolverUnrooted<Graph> SolverUnrootedType;
typedef TreeHeuristicSolverImpl<Graph> TreeHeuristicSolverImplType;
typedef TreeHeuristicSolverUnrootedImpl<Graph> TreeHeuristicSolverUnrootedImplType;
typedef TreeHeuristicSolverRootedImpl<Graph> TreeHeuristicSolverRootedImplType;
typedef SolverType::NodeSet NodeSet;
typedef TreeHeuristicSolverImplType::Options Options;

int main (int argc, char** argv)
{
  // parse command line arguments
  int verbosityLevel = 2;
  bool noPreprocess = false;
  bool analyze = false;
  std::string root;
  std::string outputFile;
  double lambda = 0;
  double a = 0;
  double fdr = 0;
  std::string nodeFile;
  std::string edgeFile;
  std::string stpFile;
  std::string stpPcstFile;
  int n = -1;
  int timeLimit = -1;
  int h = 1;

  lemon::ArgParser ap(argc, argv);

  ap
    .boolOption("version", "Show version number")
    .refOption("t", "Time limit (in seconds, default: -1)", timeLimit, false)
    .refOption("p", "Disable preprocessing", noPreprocess, false)
    .refOption("e", "Edge list file", edgeFile, false)
    .refOption("n", "Node file", nodeFile, false)
    .refOption("stp", "STP file", stpFile, false)
    .refOption("stp-pcst", "STP-PCST file", stpPcstFile, false)
    .refOption("v", "Specifies the verbosity level:\n"
                    "     0 - No output\n"
                    "     1 - Only necessary output\n"
                    "     2 - More verbose output (default)\n"
                    "     3 - Debug output", verbosityLevel, false)
    .synonym("-verbosity", "v")
    .refOption("r", "Specifies the root node (optional)", root, false)
    .refOption("lambda", "Specifies lambda", lambda, false)
    .refOption("a", "Specifies a", a, false)
    .refOption("FDR", "Specifies fdr", fdr, false)
    .refOption("o", "Output file", outputFile, false)
    .refOption("h", "Specifies heuristic\n"
                    "     0 - fixed_edge\n"
                    "     1 - random_edge (default)\n"
                    "     2 - uniform_edge\n"
                    "     3 - min_max_edge", h, false)
    .refOption("m", "Number of Monte Carlo iterations", n, false)
    .refOption("z", "Enable negative hubs sampling", analyze, false);
  ap.parse();

  if (ap.given("version"))
  {
    std::cout << "Version number: " << HEINZ_VERSION << std::endl;
    return 0;
  }
  
  if (timeLimit <= 0 && n <= 0)
  {
    std::cerr << "Please specify either -n or -t" << std::endl;
    return 1;
  }

  bool pval = ap.given("FDR") && ap.given("lambda") && ap.given("a");
  if (pval)
  {
    // check if ok
    if (!(0 <= fdr && fdr <= 1))
    {
      std::cerr << "Value of FDR should be in the range [0,1]" << std::endl;
      return 1;
    }
    if (!(0 <= lambda && lambda <= 1))
    {
      std::cerr << "Value of lambda should be in the range [0,1]" << std::endl;
      return 1;
    }
    if (!(0 <= a && a <= 1))
    {
      std::cerr << "Value of a should be in the range [0,1]" << std::endl;
      return 1;
    }
  }
  
  if (!(ap.given("n") && ap.given("e")) && !ap.given("stp") &&  !ap.given("stp-pcst"))
  {
    std::cerr << "Please specify either '-n' and '-e', or '-stp', or '-stp-pcst'" << std::endl;
    return 1;
  }
  
  Options options(static_cast<TreeHeuristicSolverImplType::EdgeHeuristic>(h),
                  analyze, n, timeLimit);

  g_verbosity = static_cast<VerbosityLevel>(verbosityLevel);

  // Construct parser
  ParserType* pParser = NULL;
  if (!stpFile.empty())
  {
    pParser = new StpParserType(stpFile);
  }
  else if (!stpPcstFile.empty())
  {
    pParser = new StpPcstParserType(stpPcstFile);
  }
  else
  {
    pParser = new MwcsParserType(nodeFile, edgeFile);
  }

  // Parse the input graph file and preprocess
  MwcsGraphType* pMwcs;
  MwcsPreprocessedGraphType* pPreprocessedMwcs = NULL;
  if (!noPreprocess)
  {
    pMwcs = pPreprocessedMwcs = new MwcsPreprocessedGraphType();
  }
  else
  {
    pMwcs = new MwcsGraphType();
  }

  if (!pMwcs->init(pParser, pval))
  {
    delete pParser;
    return 1;
  }

  // compute scores
  if (pval)
  {
    if (ap.given("a") && ap.given("lambda"))
      pMwcs->computeScores(lambda, a, fdr);
    else
      pMwcs->computeScores(fdr);
  }

  // Solve
  lemon::Timer t;

  const NodeSet rootNodeSet = pMwcs->getNodeByLabel(root);
  assert(rootNodeSet.size() == 0 || rootNodeSet.size() == 1);
  
  if (pPreprocessedMwcs)
  {
    pPreprocessedMwcs->preprocess(rootNodeSet);
  }
  SolverType* pSolver = NULL;
  if (rootNodeSet.size() == 1)
  {
    SolverRootedType* pSolverRooted = new SolverRootedType(new TreeHeuristicSolverRootedImplType(options));
    pSolverRooted->solve(*pMwcs, rootNodeSet);
    pSolver = pSolverRooted;
  }
  else
  {
    SolverUnrootedType* pSolverUnrooted = new SolverUnrootedType(new TreeHeuristicSolverUnrootedImplType(options));
    pSolverUnrooted->solve(*pMwcs);
    pSolver = pSolverUnrooted;
  }

  if (outputFile != "-" && !outputFile.empty())
  {
    std::ofstream outFile(outputFile.c_str());
    pMwcs->printHeinz(pSolver->getSolutionModule(), outFile);
    pMwcs->printModule(pSolver->getSolutionModule(), std::cout, true);
  }
  else if (outputFile == "-")
  {
    pMwcs->printHeinz(pSolver->getSolutionModule(), std::cout);
  }
  else
  {
    pMwcs->printModule(pSolver->getSolutionModule(), std::cout);
  }

  std::cerr << "Score: " << pSolver->getSolutionWeight() << std::endl;
  std::cerr << "Time: " << t.realTime() << "s" << std::endl;

  delete pSolver;
  delete pParser;
  delete pMwcs;

  return 0;
}
