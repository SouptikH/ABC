/*
 * Driver.h
 *
 *  Created on: Nov 17, 2014
 *      Author: baki
 */

#ifndef SRC_DRIVER_H_
#define SRC_DRIVER_H_

#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <utility>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/multiprecision/detail/et_ops.hpp>
#include <boost/multiprecision/detail/number_base.hpp>
#include <boost/multiprecision/detail/number_compare.hpp>
#include <boost/multiprecision/number.hpp>
#include <glog/logging.h>

#include "parser/location.hh"
#include "parser/parser.hpp"
#include "parser/Scanner.h"
#include "smt/ast.h"
#include "smt/typedefs.h"
#include "solver/Ast2Dot.h"
#include "solver/ConstraintInformation.h"
#include "solver/ConstraintSolver.h"
#include "solver/ConstraintSorter.h"
#include "solver/DependencySlicer.h"
#include "solver/EquivalenceGenerator.h"
#include "solver/FormulaOptimizer.h"
#include "solver/ImplicationRunner.h"
#include "solver/Initializer.h"
#include "solver/options/Solver.h"
#include "solver/SymbolTable.h"
#include "solver/SyntacticOptimizer.h"
#include "solver/SyntacticProcessor.h"
#include "solver/Value.h"
#include "theory/ArithmeticFormula.h"
#include "theory/BinaryIntAutomaton.h"
#include "theory/IntAutomaton.h"
#include "theory/MultiTrackAutomaton.h"
#include "theory/options/Theory.h"
#include "theory/StringAutomaton.h"
#include "theory/StringRelation.h"

namespace Vlab {
namespace SMT {
class location;
} /* namespace SMT */
} /* namespace Vlab */

namespace Vlab {

class Driver {
public:
  Driver();
  ~Driver();

  void initializeABC(int log_level);
  // Error handling.
  void error(const Vlab::SMT::location& l, const std::string& m);
  void error(const std::string& m);
  int parse(std::istream* in = &std::cin);
  void ast2dot(std::string file_name);
  void ast2dot(std::ostream* out);
//	void collectStatistics();
  void initializeSolver();
  void solve();
  bool isSatisfiable();
  boost::multiprecision::cpp_int Count(std::string var_name, const double bound, bool count_less_than_or_equal_to_bound = true);
  boost::multiprecision::cpp_int Count(const double bound, bool count_less_than_or_equal_to_bound = true);
  boost::multiprecision::cpp_int SymbolicCount(std::string var_name, const double bound, bool count_less_than_or_equal_to_bound = true);
  boost::multiprecision::cpp_int SymbolicCount(const int bound, bool count_less_than_or_equal_to_bound = true);

  void printResult(Solver::Value_ptr value, std::ostream& out);
  void inspectResult(Solver::Value_ptr value, std::string file_name);
  std::map<SMT::Variable_ptr, Solver::Value_ptr> getSatisfyingVariables();
  std::map<std::string, std::string> getSatisfyingExamples();
  void reset();
//	void solveAst();

  void setOption(Option::Name option, bool value);
  void setOption(Option::Name option, std::string value);

  SMT::Variable_ptr get_smc_query_variable();
  void test();

  SMT::Script_ptr script_;
  Solver::SymbolTable_ptr symbol_table_;
  Solver::ConstraintInformation_ptr constraint_information_;

  int trace_parsing_ = 0;
  int trace_scanning_ = 0;
  std::string file_;

private:
  static bool IS_LOGGING_INITIALIZED;

};

}

#endif /* SRC_DRIVER_H_ */
