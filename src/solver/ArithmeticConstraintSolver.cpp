/*
 * ArithmeticConstraintSolver.cpp
 *
 *  Created on: Nov 1, 2015
 *      Author: baki
 */

#include "ArithmeticConstraintSolver.h"

namespace Vlab {
namespace Solver {

using namespace SMT;
using namespace Theory;

const int ArithmeticConstraintSolver::VLOG_LEVEL = 11;

ArithmeticConstraintSolver::ArithmeticConstraintSolver(Script_ptr script, SymbolTable_ptr symbol_table,
                                                       ConstraintInformation_ptr constraint_information,
                                                       bool use_signed_integers)
    : AstTraverser(script),
      use_unsigned_integers_ { not use_signed_integers },
      symbol_table_(symbol_table),
      constraint_information_(constraint_information),
      arithmetic_formula_generator_(script, symbol_table, constraint_information) {
  setCallbacks();
}

ArithmeticConstraintSolver::~ArithmeticConstraintSolver() {
}


void ArithmeticConstraintSolver::start(Visitable_ptr node) {
  DVLOG(VLOG_LEVEL) << "Arithmetic constraint solving starts at node: " << node;
  this->Visitor::visit(node);
  end();
}

void ArithmeticConstraintSolver::start() {
  DVLOG(VLOG_LEVEL) << "Arithmetic constraint solving starts at root";
  visitScript(root_);
  end();
}

void ArithmeticConstraintSolver::end() {
  arithmetic_formula_generator_.end();
}

void ArithmeticConstraintSolver::collect_arithmetic_constraint_info() {
  arithmetic_formula_generator_.start();
  string_terms_map_ = arithmetic_formula_generator_.get_string_terms_map();
}

/**
 * TODO move group updating inside AND and OR
 */
void ArithmeticConstraintSolver::setCallbacks() {
  auto term_callback = [this] (Term_ptr term) -> bool {
    switch (term->type()) {
      case Term::Type::NOT:
      case Term::Type::EQ:
      case Term::Type::NOTEQ:
      case Term::Type::GT:
      case Term::Type::GE:
      case Term::Type::LT:
      case Term::Type::LE:
      case Term::Type::QUALIDENTIFIER: {
        auto formula = arithmetic_formula_generator_.get_term_formula(term);
        if (formula != nullptr) {
          DVLOG(VLOG_LEVEL) << "Linear Arithmetic Equation: " << *formula << "@" << term;
          auto binary_int_auto = IntegerAutomaton::MakeAutomaton(formula->clone(), use_unsigned_integers_);
          auto result = new Value(binary_int_auto);
          set_term_value(term, result);
          // once we solve an atomic linear integer arithmetic constraint,
          // we delete its formula to avoid solving it again.
          // Atomic arithmetic constraints solved precisely,
          // mixed constraints handled without resolving arithmetic part
          arithmetic_formula_generator_.clear_term_formula(term);
        }
        break;
      }
      default:
      break;
    }
    return false;
  };

  auto command_callback = [](Command_ptr command) -> bool {
    if (Command::Type::ASSERT == command->getType()) {
      return true;
    }
    return false;
  };

  setCommandPreCallback(command_callback);
  setTermPreCallback(term_callback);
}

void ArithmeticConstraintSolver::visitScript(Script_ptr script) {
  symbol_table_->push_scope(script);
  visit_children_of(script);
  symbol_table_->pop_scope();  // global scope, it is reachable via script pointer all the time
}

void ArithmeticConstraintSolver::visitAssert(Assert_ptr assert_command) {
  DVLOG(VLOG_LEVEL) << "visit: " << *assert_command;
  AstTraverser::visit(assert_command->term);
}

void ArithmeticConstraintSolver::visitAnd(And_ptr and_term) {
  if (not constraint_information_->is_component(and_term)) {
    DVLOG(VLOG_LEVEL) << "visit children of non-component start: " << *and_term << "@" << and_term;
    visit_children_of(and_term);
    DVLOG(VLOG_LEVEL) << "visit children of non-component end: " << *and_term << "@" << and_term;
    return;
  }

  if (not constraint_information_->has_arithmetic_constraint(and_term)) {
    return;
  }

  DVLOG(VLOG_LEVEL) << "visit children of component start: " << *and_term << "@" << and_term;

  bool is_satisfiable = true;
  bool has_arithmetic_formula = false;

  std::string group_name = arithmetic_formula_generator_.get_term_group_name(and_term);
  Value_ptr and_value = nullptr;

  for (auto term : *(and_term->term_list)) {
    auto formula = arithmetic_formula_generator_.get_term_formula(term);
    // Do not visit child or terms here, handle them in POSTVISIT AND
    if (formula != nullptr and (dynamic_cast<Or_ptr>(term) == nullptr)) {
      has_arithmetic_formula = true;
      visit(term);
      auto param = get_term_value(term);
      is_satisfiable = param->is_satisfiable();
      if (is_satisfiable) {
        if (and_value == nullptr) {
          and_value = param->clone();
        } else {
          auto old_value = and_value;
          and_value = and_value->intersect(param);
          delete old_value;
          is_satisfiable = and_value->is_satisfiable();
        }
      }
      clear_term_value(term);
      if (not is_satisfiable) {
        break;
      }
    }
  }

  DVLOG(VLOG_LEVEL) << "visit children of component end: " << *and_term << "@" << and_term;

  DVLOG(VLOG_LEVEL) << "post visit component start: " << *and_term << "@" << and_term;

  /**
   * If and term does not have arithmetic formula, but we end up being here:
   * 1) And term is under a disjunction and other disjunctive terms has arithmetic formula.
   *  Here, variables appearing in arithmetic term will be unconstrained.
   * 2) We are visited and term second time for some mixed constraints, for this we do an unnecessary
   *  intersection below with any string, we can avoid that with more checks later!!!
   */
  if (and_value == nullptr and (not has_arithmetic_formula)) {
    auto group_formula = arithmetic_formula_generator_.get_group_formula(group_name);
    and_value = new Value(Theory::IntegerAutomaton::MakeAnyInt(group_formula->clone(), use_unsigned_integers_));
    has_arithmetic_formula = true;
    is_satisfiable = true;
  }

  if (has_arithmetic_formula) {
    if (is_satisfiable) {
      symbol_table_->IntersectValue(group_name, and_value);  // update value
    } else {
      auto group_formula = arithmetic_formula_generator_.get_group_formula(group_name);
      auto value = new Value(Theory::IntegerAutomaton::MakePhi(group_formula->clone(), use_unsigned_integers_));
      symbol_table_->set_value(group_name, value);
    }
    delete and_value;
  }
  DVLOG(VLOG_LEVEL) << "post visit component end: " << *and_term << "@" << and_term;
}

/**
 * 1) Update group value at each scope
 * 2) Update result (union of scopes) after all
 */
void ArithmeticConstraintSolver::visitOr(Or_ptr or_term) {
  if (not constraint_information_->is_component(or_term)) {  // a rare case, @deprecated
    DVLOG(VLOG_LEVEL) << "visit children of non-component start: " << *or_term << "@" << or_term;
    visit_children_of(or_term);
    DVLOG(VLOG_LEVEL) << "visit children of non-component end: " << *or_term << "@" << or_term;
    return;
  }

  if (not constraint_information_->has_arithmetic_constraint(or_term)) {
    return;
  }

  DVLOG(VLOG_LEVEL) << "visit children of component start: " << *or_term << "@" << or_term;
  bool is_satisfiable = false;
  std::string group_name = arithmetic_formula_generator_.get_term_group_name(or_term);
  Value_ptr or_value = nullptr;
  for (auto term : *(or_term->term_list)) {
    auto formula = arithmetic_formula_generator_.get_term_formula(term);
    if (formula != nullptr and (dynamic_cast<And_ptr>(term) == nullptr)) {
      symbol_table_->push_scope(term);
      visit(term);
      auto param = get_term_value(term);
      is_satisfiable = param->is_satisfiable() or is_satisfiable;
      if (is_satisfiable) {
        if (or_value == nullptr) {
          or_value = param->clone();
        } else {
          auto old_value = or_value;
          or_value = or_value->union_(param);
          delete old_value;
        }
      }
      clear_term_value(term);
      symbol_table_->pop_scope();
    }
  }

  DVLOG(VLOG_LEVEL) << "visit children of component end: " << *or_term << "@" << or_term;

  DVLOG(VLOG_LEVEL) << "post visit component start: " << *or_term << "@" << or_term;

  if (constraint_information_->has_arithmetic_constraint(or_term)) {
    if (is_satisfiable) {
      // scope already reads value from upper scope
      // implicit intersection is already done
//      symbol_table_->IntersectValue(group_name, or_value);  // update value for union, this is upper scope
      symbol_table_->set_value(group_name, or_value);
    } else {
      auto group_formula = arithmetic_formula_generator_.get_group_formula(group_name);
      auto value = new Value(Theory::IntegerAutomaton::MakePhi(group_formula->clone(), use_unsigned_integers_));
      symbol_table_->set_value(group_name, value);
      delete or_value; // nullptr safe
    }
  }

  DVLOG(VLOG_LEVEL) << "post visit component end: " << *or_term << "@" << or_term;
}

/**
 * Collects the result of the visitAnd all, does not recurses again
 */
void ArithmeticConstraintSolver::postVisitAnd(And_ptr and_term) {
  DVLOG(VLOG_LEVEL) << "collect child results start: " << *and_term << "@" << and_term;

  bool is_satisfiable = true;
  bool has_arithmetic_formula = false;

  std::string group_name = arithmetic_formula_generator_.get_term_group_name(and_term);
  Value_ptr and_value = nullptr;

  for (auto term : *(and_term->term_list)) {
    auto formula = arithmetic_formula_generator_.get_term_formula(term);
    /**
     * In previous visit, automata for arithmetic constraints are created and
     * formulae for them are deleted.
     * For sub or terms, constraint solver recurses into and here we don't
     * need to visit them, a value is already computed for them,
     * grabs them from symbol table
     */
    if (formula != nullptr) {
      has_arithmetic_formula = true;
      auto param = get_term_value(term);
      is_satisfiable = param->is_satisfiable();
      if (is_satisfiable) {
        if (and_value == nullptr) {
          and_value = param->clone();
        } else {
          auto old_value = and_value;
          and_value = and_value->intersect(param);
          delete old_value;
          is_satisfiable = and_value->is_satisfiable();
        }
      }
      clear_term_value(term);
      if (not is_satisfiable) {
        break;
      }
    }
  }

  DVLOG(VLOG_LEVEL) << "collect child results end: " << *and_term << "@" << and_term;

  DVLOG(VLOG_LEVEL) << "update result start: " << *and_term << "@" << and_term;

  /**
   * If and term does not have arithmetic formula, but we end up being here:
   * 1) And term is under a disjunction and other disjunctive terms has arithmetic formula.
   *  Here, variables appearing in arithmetic term will be unconstrained.
   * 2) We are visited and term second time for some mixed constraints, for this we do an unnecessary
   *  intersection below with any string, we can avoid that with more checks later!!!
   */
  if (and_value == nullptr and (not has_arithmetic_formula)) {
    auto group_formula = arithmetic_formula_generator_.get_group_formula(group_name);
    and_value = new Value(Theory::IntegerAutomaton::MakeAnyInt(group_formula->clone(), use_unsigned_integers_));
    has_arithmetic_formula = true;
    is_satisfiable = true;
  }

  if (has_arithmetic_formula) {
    if (is_satisfiable) {
      symbol_table_->IntersectValue(group_name, and_value);  // update value
    } else {
      auto group_formula = arithmetic_formula_generator_.get_group_formula(group_name);
      auto value = new Value(Theory::IntegerAutomaton::MakePhi(group_formula->clone(), use_unsigned_integers_));
      symbol_table_->set_value(group_name, value);
    }
    delete and_value;
  }
  DVLOG(VLOG_LEVEL) << "update result end: " << *and_term << "@" << and_term;
}

void ArithmeticConstraintSolver::postVisitOr(Or_ptr or_term) {
  DVLOG(VLOG_LEVEL) << "collect child results start: " << *or_term << "@" << or_term;
  visitOr(or_term);
  DVLOG(VLOG_LEVEL) << "collect child results end: " << *or_term << "@" << or_term;
}

std::string ArithmeticConstraintSolver::get_int_variable_name(SMT::Term_ptr term) {
  Term_ptr key = term;
  auto it1 = term_value_index_.find(term);
  if (it1 != term_value_index_.end()) {
    key = it1->second;
  }

  return symbol_table_->get_var_name_for_node(key, Variable::Type::INT);
}

Value_ptr ArithmeticConstraintSolver::get_term_value(Term_ptr term) {
  auto it = term_values_.find(term);
  if (it != term_values_.end()) {
    return it->second;
  }
  std::string group_name = arithmetic_formula_generator_.get_term_group_name(term);

  if (not group_name.empty()) {
    return symbol_table_->get_value(group_name);
  }

  return nullptr;
}

/**
 * Term values are only stored for atomic constraints
 */
bool ArithmeticConstraintSolver::set_term_value(Term_ptr term, Value_ptr value) {
  auto result = term_values_.insert( { term, value });
  if (result.second == false) {
    LOG(FATAL)<< "Term automaton is already computed: " << *term << "@" << term;
  }
//  std::string group_name = arithmetic_formula_generator_.get_term_group_name(term);
//  if(!group_name.empty()) {
//    symbol_table_->set_value(group_name,value);
//    return true;
//  }
  return result.second;
}

bool ArithmeticConstraintSolver::set_group_value(Term_ptr term, Value_ptr value) {
  std::string group_name = arithmetic_formula_generator_.get_term_group_name(term);
  return symbol_table_->set_value(group_name, value);
}

void ArithmeticConstraintSolver::clear_term_values() {
  for (auto& entry : term_values_) {
    delete entry.second;
  }

  term_values_.clear();
}

/**
 * We don't need an atomic term value ones we compute it.
 */
void ArithmeticConstraintSolver::clear_term_value(Term_ptr term) {
  auto it = term_values_.find(term);
  if (it != term_values_.end()) {
    delete it->second;
    term_values_.erase(it);
  }
}

bool ArithmeticConstraintSolver::has_string_terms(Term_ptr term) {
  return (string_terms_map_.find(term) != string_terms_map_.end());
}

TermList& ArithmeticConstraintSolver::get_string_terms_in(Term_ptr term) {
  return string_terms_map_[term];
}

std::map<SMT::Term_ptr, SMT::TermList>& ArithmeticConstraintSolver::get_string_terms_map() {
  return string_terms_map_;
}

} /* namespace Solver */
} /* namespace Vlab */
