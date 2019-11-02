/*
 * IntAutomaton.h
 *
 *  Created on: Jun 24, 2015
 *      Author: baki
 */

#ifndef THEORY_INTAUTOMATON_H_
#define THEORY_INTAUTOMATON_H_

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <stack>
#include <vector>

#include <glog/logging.h>
//#include <mona/bdd.h>
//#include <mona/dfa.h>

#include "Automaton.h"
#include "ArithmeticFormula.h"
#include "UnaryAutomaton.h"
#include "StringAutomaton.h"
#include "BinaryIntAutomaton.h"


namespace Vlab {
namespace Theory {

class IntAutomaton;
typedef IntAutomaton* IntAutomaton_ptr;

/**
 * A compatible version with string automaton
 */
class IntAutomaton: public Automaton {
public:
  IntAutomaton(DFA_ptr);
  IntAutomaton(DFA_ptr, int num_of_variables);
  IntAutomaton(DFA_ptr, bool has_negative_1);
  IntAutomaton(DFA_ptr, bool has_negative_1, int num_of_variables);
  IntAutomaton(const IntAutomaton&);
  virtual ~IntAutomaton();

  virtual IntAutomaton_ptr clone() const;
  IntAutomaton_ptr MakeAutomaton(DFA_ptr dfa, Formula_ptr formula, const int number_of_variables);
  static IntAutomaton_ptr makePhi(int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeZero(int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeAnyInt(int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeInt(int value, int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeIntLessThan(int value, int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeIntLessThanOrEqual(int value, int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeIntGreaterThan(int value, int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeIntGreaterThanOrEqual(int value, int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeIntRange(int start, int end, int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);
  static IntAutomaton_ptr makeInts(std::vector<int> values, int num_of_variables = IntAutomaton::DEFAULT_NUM_OF_VARIABLES);

  void setMinus1(bool has_minus_one);
  bool hasNegative1();
  IntAutomaton_ptr Complement();
  IntAutomaton_ptr Union(int value);
  IntAutomaton_ptr Union(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr Intersect(int value);
  IntAutomaton_ptr Intersect(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr Difference(int value);
  IntAutomaton_ptr Difference(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr uminus();

  IntAutomaton_ptr plus(int value);
  IntAutomaton_ptr plus(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr times(int value);
  IntAutomaton_ptr minus(int value);
  IntAutomaton_ptr minus(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr substractFrom(int value);

  int getMaxAcceptedInt();
  int getMinAcceptedInt();

  bool isGreaterThan(int value);
  bool isGreaterThan(IntAutomaton_ptr other_auto);
  bool isGreaterThanOrEqual(int value);
  bool isGreaterThanOrEqual(IntAutomaton_ptr other_auto);
  bool isLessThan(int value);
  bool isLessThan(IntAutomaton_ptr other_auto);
  bool isLessThanOrEqual(int value);
  bool isLessThanOrEqual(IntAutomaton_ptr other_auto);

  IntAutomaton_ptr restrictTo(IntAutomaton_ptr other_value);
  IntAutomaton_ptr restrictGreaterThanTo(int value);
  IntAutomaton_ptr restrictGreaterThanTo(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr restrictGreaterThanOrEqualTo(int value);
  IntAutomaton_ptr restrictGreaterThanOrEqualTo(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr restrictLessThanTo(int value);
  IntAutomaton_ptr restrictLessThanTo(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr restrictLessThanOrEqualTo(int value);
  IntAutomaton_ptr restrictLessThanOrEqualTo(IntAutomaton_ptr other_auto);

  bool checkEquivalance(IntAutomaton_ptr other_auto);
  bool isEmptyLanguage();
  bool hasZero();
  bool isZero();
  bool isAcceptingSingleInt();
  int getAnAcceptingInt();

  UnaryAutomaton_ptr toUnaryAutomaton();

  ArithmeticFormula_ptr GetFormula();
  void SetFormula(ArithmeticFormula_ptr);

  static const int INFINITE;
  static int DEFAULT_NUM_OF_VARIABLES;
protected:
  IntAutomaton_ptr __plus(IntAutomaton_ptr other_auto);
  IntAutomaton_ptr __minus(IntAutomaton_ptr other_auto);

  void decide_counting_schema(Eigen::SparseMatrix<BigInteger>& count_matrix) override;

  ArithmeticFormula_ptr formula_;
  bool has_negative_1;
private:
  static const int VLOG_LEVEL;
};

} /* namespace Theory */
} /* namespace Vlab */

#endif /* THEORY_INTAUTOMATON_H_ */
