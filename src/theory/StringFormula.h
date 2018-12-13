/*
 * StringFormula.h
 *
 *  Created on: Jan 22, 2017
 *      Author: baki
 */

#ifndef THEORY_STRINGFORMULA_H_
#define THEORY_STRINGFORMULA_H_

#include <cmath>
#include <cstdlib>
#include <locale>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>
#include <cereal/types/map.hpp>

#include "Formula.h"
#include "../smt/ast.h"

namespace Vlab {
namespace Theory {

class StringFormula;
using StringFormula_ptr = StringFormula*;

class StringFormula : public Formula {
 public:
  enum class Type : int {
    NONE = 0, EQ, NOTEQ, GT, GE, LT, LE, DIFFERENCE, VAR, CHARAT,EQ_CHARAT,NOTEQ_CHARAT,
    		GT_CHARAT,GE_CHARAT,LT_CHARAT,LE_CHARAT,
        STRING_CONSTANT, REGEX_CONSTANT, INTEGER_CONSTANT, EQ_NO_LAMBDA, EQ_ONLY_LAMBDA, SUBSTRING,
        BEGINS, BEGINS_SUBSTRING, NOTBEGINS, NOTBEGINS_SUBSTRING, CONCAT_VAR_CONSTANT, INTERSECT, UNION, NONRELATIONAL
  };

  StringFormula();
  virtual ~StringFormula();

  StringFormula(const StringFormula&);
  virtual StringFormula_ptr clone() const;

  template <class Archive>
  void save(Archive& ar) const {
    ar(type_);
    ar(variable_coefficient_map_);
  }

  template <class Archive>
  void load(Archive& ar) {
    ar(type_);
    ar(variable_coefficient_map_);
  }

  virtual std::string str() const;

  virtual StringFormula_ptr Intersect(Formula_ptr);
	virtual StringFormula_ptr Union(Formula_ptr);
	virtual StringFormula_ptr Complement();

  void SetType(Type type);
  StringFormula::Type GetType() const;
  std::string GetConstant() const;
  std::string GetConstant2() const;
  void SetConstant(std::string constant);
  void SetConstant2(std::string constant);
  bool IsConstant() const;

  bool HasRelationToMixedTerm(const std::string var_name) const;
  void AddRelationToMixedTerm(const std::string var_name, const StringFormula::Type relation, const SMT::Term_ptr term);
  std::pair<StringFormula::Type, SMT::Term_ptr> GetRelationToMixedTerm(const std::string var_name) const;
  virtual bool UpdateMixedConstraintRelations();

  int CountOnes(unsigned long n) const;
  virtual void MergeVariables(Formula_ptr);

  friend std::ostream& operator<<(std::ostream& os, const StringFormula& formula);

protected:
  bool GetVarNamesIfEqualityOfTwoVars(std::string &v1, std::string &v2);

  StringFormula::Type type_;
  std::string constant_;
  // BAD! Rework instead of this hack!
  std::string constant2_;

  // TODO a quick solution for a restricted set of cases in mixed constraints
  // generalize it as much as possible
  std::map<std::string, std::pair<StringFormula::Type, SMT::Term_ptr>> mixed_terms_;

private:
  static const int VLOG_LEVEL;
};

} /* namespace Theory */
} /* namespace Vlab */

#endif /* SRC_THEORY_STRINGFORMULA_H_ */
