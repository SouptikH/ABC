/*
 * FormulaOptimizer.cpp
 *
 *  Created on: May 12, 2015
 *      Author: baki
 */

#include "FormulaOptimizer.h"

namespace Vlab {
namespace Solver {

using namespace SMT;

const int FormulaOptimizer::VLOG_LEVEL = 14;

FormulaOptimizer::FormulaOptimizer(Script_ptr script, SymbolTable_ptr symbol_table)
        : AstTraverser(script), symbol_table_(symbol_table), delete_term_ {false} {
  setCallbacks();
}

FormulaOptimizer::~FormulaOptimizer() {
}

void FormulaOptimizer::start() {
  visitScript(root_);
  end();
}

void FormulaOptimizer::end() {
//  SyntacticOptimizer syntactic_optimizer(root_, symbol_table_);
//  syntactic_optimizer.start();
}

void FormulaOptimizer::setCallbacks() {
  auto term_callback = [] (Term_ptr term) -> bool {
    return false;
  };
  setTermPreCallback(term_callback);
}

void FormulaOptimizer::visitAssert(Assert_ptr assert_command) {
  visit(assert_command->term);
}

void FormulaOptimizer::visitAnd(And_ptr and_term) {
  DVLOG(VLOG_LEVEL) << "visit children start: " << *and_term << "@" << and_term;
  for (auto iter = and_term->term_list->begin(); iter != and_term->term_list->end();) {
    visit(*iter);
    if (delete_term_) {
      delete (*iter);
      iter = and_term->term_list->erase(iter);
    } else {
      iter++;
    }
    delete_term_ = false;
  }
  term_strs_.clear();
  DVLOG(VLOG_LEVEL) << "visit children end: " << *and_term << "@" << and_term;

  // TODO add and term check
}

void FormulaOptimizer::visitOr(Or_ptr or_term) {
  DVLOG(VLOG_LEVEL) << "visit children start: " << *or_term << "@" << or_term;
  for (auto iter = or_term->term_list->begin(); iter != or_term->term_list->end();) {
  	auto temp_term_strs = term_strs_;
  	term_strs_.clear();
    visit(*iter);
    if (delete_term_) {
      delete (*iter);
      iter = or_term->term_list->erase(iter);
    } else {
      iter++;
    }
    delete_term_ = false;
    term_strs_ = temp_term_strs;
  }
  term_strs_.clear();
  DVLOG(VLOG_LEVEL) << "visit children end: " << *or_term << "@" << or_term;
  // TODO add or term check
}


void FormulaOptimizer::visitEq(Eq_ptr eq_term) {

	Term_ptr* reference_term = top();
	if(eq_term->left_term->type() == Term::Type::TERMCONSTANT and eq_term->right_term->type() == Term::Type::QUALIDENTIFIER) {
		TermConstant_ptr term_constant = dynamic_cast<TermConstant_ptr>(eq_term->left_term);

		if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "true") {
			*reference_term = eq_term->right_term;
			eq_term->right_term = nullptr;
			delete eq_term; eq_term = nullptr;
			return;
		} else if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "false") {
			*reference_term = new Not(eq_term->right_term);
			eq_term->right_term = nullptr;
			delete eq_term; eq_term = nullptr;
			return;
		}
	} else if(eq_term->left_term->type() == Term::Type::QUALIDENTIFIER and eq_term->right_term->type() == Term::Type::TERMCONSTANT) {
		TermConstant_ptr term_constant = dynamic_cast<TermConstant_ptr>(eq_term->right_term);
		if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "true") {
			*reference_term = eq_term->left_term;
			eq_term->left_term = nullptr;
			delete eq_term; eq_term = nullptr;
			return;
		} else if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "false") {
			*reference_term = new Not(eq_term->left_term);
			eq_term->left_term = nullptr;
			delete eq_term; eq_term = nullptr;
			return;
		}
	}

  const std::string left_expr = Ast2Dot::toString(eq_term->left_term);
  const std::string right_expr = Ast2Dot::toString(eq_term->right_term);

  std::stringstream ss;
  ss << *eq_term;
  if (right_expr < left_expr) {
//    Term_ptr tmp = eq_term->left_term;
//    eq_term->left_term = eq_term->right_term;
//    eq_term->right_term = tmp;
    ss << right_expr << left_expr;
  } else {
    ss << left_expr << right_expr;
  }

  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitNotEq(NotEq_ptr not_eq_term) {

	Term_ptr* reference_term = top();
	if(not_eq_term->left_term->type() == Term::Type::TERMCONSTANT and not_eq_term->right_term->type() == Term::Type::QUALIDENTIFIER) {
		TermConstant_ptr term_constant = dynamic_cast<TermConstant_ptr>(not_eq_term->left_term);
		if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "false") {
			*reference_term = not_eq_term->right_term;
			not_eq_term->right_term = nullptr;
			delete not_eq_term; not_eq_term = nullptr;
			return;
		} else if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "true") {
			*reference_term = new Not(not_eq_term->right_term);
			not_eq_term->right_term = nullptr;
			delete not_eq_term; not_eq_term = nullptr;
			return;
		}
	} else if(not_eq_term->left_term->type() == Term::Type::QUALIDENTIFIER and not_eq_term->right_term->type() == Term::Type::TERMCONSTANT) {
		TermConstant_ptr term_constant = dynamic_cast<TermConstant_ptr>(not_eq_term->right_term);
		if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "false") {
			*reference_term = not_eq_term->left_term;
			not_eq_term->left_term = nullptr;
			delete not_eq_term; not_eq_term = nullptr;
			return;
		} else if(term_constant->getValueType() == Primitive::Type::BOOL and term_constant->getValue() == "true") {
			*reference_term = new Not(not_eq_term->left_term);
			not_eq_term->left_term = nullptr;
			delete not_eq_term; not_eq_term = nullptr;
			return;
		}
	}

  const std::string left_expr = Ast2Dot::toString(not_eq_term->left_term);
  const std::string right_expr = Ast2Dot::toString(not_eq_term->right_term);

  std::stringstream ss;
  ss << *not_eq_term;
  if (right_expr < left_expr) {
//    Term_ptr tmp = not_eq_term->left_term;
//    not_eq_term->left_term = not_eq_term->right_term;
//    not_eq_term->right_term = tmp;
    ss << right_expr << left_expr;
  } else {
    ss << left_expr << right_expr;
  }


  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitGt(Gt_ptr gt_term) {
  const std::string left_expr = Ast2Dot::toString(gt_term->left_term);
  const std::string right_expr = Ast2Dot::toString(gt_term->right_term);

  std::stringstream ss;
  ss << *gt_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitGe(Ge_ptr ge_term) {
  const std::string left_expr = Ast2Dot::toString(ge_term->left_term);
  const std::string right_expr = Ast2Dot::toString(ge_term->right_term);

  std::stringstream ss;
  ss << *ge_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitLt(Lt_ptr lt_term) {
  const std::string left_expr = Ast2Dot::toString(lt_term->left_term);
  const std::string right_expr = Ast2Dot::toString(lt_term->right_term);

  std::stringstream ss;
  ss << *lt_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitLe(Le_ptr le_term) {
  const std::string left_expr = Ast2Dot::toString(le_term->left_term);
  const std::string right_expr = Ast2Dot::toString(le_term->right_term);

  std::stringstream ss;
  ss << *le_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitIn(In_ptr in_term) {
  const std::string left_expr = Ast2Dot::toString(in_term->left_term);
  const std::string right_expr = Ast2Dot::toString(in_term->right_term);

  std::stringstream ss;
  ss << *in_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitNotIn(NotIn_ptr not_in_term) {
  const std::string left_expr = Ast2Dot::toString(not_in_term->left_term);
  const std::string right_expr = Ast2Dot::toString(not_in_term->right_term);

  std::stringstream ss;
  ss << *not_in_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitContains(Contains_ptr contains_term) {
  const std::string left_expr = Ast2Dot::toString(contains_term->subject_term);
  const std::string right_expr = Ast2Dot::toString(contains_term->search_term);

  std::stringstream ss;
  ss << *contains_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitNotContains(NotContains_ptr not_contains_term) {
  const std::string left_expr = Ast2Dot::toString(not_contains_term->subject_term);
  const std::string right_expr = Ast2Dot::toString(not_contains_term->search_term);

  std::stringstream ss;
  ss << *not_contains_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitBegins(Begins_ptr begins_term) {
  const std::string left_expr = Ast2Dot::toString(begins_term->subject_term);
  const std::string right_expr = Ast2Dot::toString(begins_term->search_term);

  std::stringstream ss;
  ss << *begins_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitNotBegins(NotBegins_ptr not_begins_term) {
  const std::string left_expr = Ast2Dot::toString(not_begins_term->subject_term);
  const std::string right_expr = Ast2Dot::toString(not_begins_term->search_term);

  std::stringstream ss;
  ss << *not_begins_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitEnds(Ends_ptr ends_term) {
  const std::string left_expr = Ast2Dot::toString(ends_term->subject_term);
  const std::string right_expr = Ast2Dot::toString(ends_term->search_term);

  std::stringstream ss;
  ss << *ends_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

void FormulaOptimizer::visitNotEnds(NotEnds_ptr not_ends_term) {
  const std::string left_expr = Ast2Dot::toString(not_ends_term->subject_term);
  const std::string right_expr = Ast2Dot::toString(not_ends_term->search_term);

  std::stringstream ss;
  ss << *not_ends_term << left_expr << right_expr;
  const std::string term_expr = ss.str();

  if (term_strs_[term_expr]) {
    delete_term_ = true;
  }
  term_strs_[term_expr] = true;
}

} /* namespace Solver */
} /* namespace Vlab */
