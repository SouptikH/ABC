/*
 * UnaryAutomaton.cpp
 *
 *  Created on: Nov 5, 2015
 *      Author: baki
 *   Copyright: Copyright 2015 The ABC Authors. All rights reserved. 
 *              Use of this source code is governed license that can
 *              be found in the COPYING file.
 */

#include "UnaryAutomaton.h"

#include "BinaryIntAutomaton.h"
#include "IntAutomaton.h"
#include "StringAutomaton.h"

namespace Vlab
{
  namespace Theory
  {

    const int UnaryAutomaton::VLOG_LEVEL = 9;

    UnaryAutomaton::UnaryAutomaton(const Libs::MONALib::DFA_ptr dfa, const int number_of_bdd_variables)
        : Automaton(dfa, 1)
    {
    }

    UnaryAutomaton::UnaryAutomaton(const UnaryAutomaton& other)
        : Automaton(other)
    {
    }

    UnaryAutomaton::~UnaryAutomaton()
    {
    }

    UnaryAutomaton_ptr UnaryAutomaton::Clone() const
    {
      UnaryAutomaton_ptr cloned_auto = new UnaryAutomaton(*this);
      DVLOG(VLOG_LEVEL) << cloned_auto->id_ << " = [" << this->id_ << "]->Clone()";
      return cloned_auto;
    }

    UnaryAutomaton_ptr UnaryAutomaton::MakeAutomaton(const Libs::MONALib::DFA_ptr dfa, const int number_of_variables) const
    {
      return new UnaryAutomaton(dfa, number_of_variables);
    }

    std::string UnaryAutomaton::Str() const
    {
      std::stringstream ss;
      ss << "UnaryAutomaton[" << id_ << "]";
      return ss.str();
    }

    SemilinearSet_ptr UnaryAutomaton::GetSemilinearSet()
    {
      int cycle_head_state = -1;
      int current_state = this->GetInitialState();
      int sink_state = this->GetSinkState();

      if (sink_state == current_state)
      {
        return new SemilinearSet();
      }

      std::vector<int> states;
      std::map<int, int> values;

      // loop over all states except for sink state
      for (int s = 0; s < this->GetNumberOfStates() - 1; s++)
      {
        values[current_state] = s;
        states.push_back(current_state);

        for (int next_state : this->GetNextStates(current_state))
        {
          if (next_state != sink_state)
          {
            if (values.find(next_state) != values.end())
            {
              cycle_head_state = next_state;
              break;
            }
            current_state = next_state;
          }
        }

        if (cycle_head_state != -1)
        {
          break;
        }
      }

      SemilinearSet_ptr semilinear_set = new SemilinearSet();
      int cycle_head_value = 0;
      bool is_in_cycle = this->IsInitialState(cycle_head_state);

      for (auto state : states)
      {
        if (not is_in_cycle)
        {
          if (state == cycle_head_state)
          {
            is_in_cycle = true;
            cycle_head_value = values[state];
            if (this->IsAcceptingState(state))
            {
              semilinear_set->add_periodic_constant(0);
            }
          }
          else
          {
            if (this->IsAcceptingState(state))
            {
              semilinear_set->add_constant(values[state]);
            }
          }
        }
        else
        {
          if (this->IsAcceptingState(state))
          {
            semilinear_set->add_periodic_constant(values[state] - cycle_head_value);
          }
        }
      }

      semilinear_set->set_cycle_head(cycle_head_value);
      int period = (cycle_head_state == -1) ? 0 : values[states.back()] - cycle_head_value + 1;
      semilinear_set->set_period(period);

      DVLOG(VLOG_LEVEL) << *semilinear_set;
      DVLOG(VLOG_LEVEL) << "semilinear set = [" << this->id_ << "]->GetSemilinearSet()";

      return semilinear_set;
    }

    IntAutomaton_ptr UnaryAutomaton::ConvertToIntAutomaton(int number_of_bdd_variables, bool add_minus_one)
    {
      LOG(FATAL) << "reimplement me";
      return nullptr;
//      int* indices = Libs::MONALib::GetBddVariableIndices(number_of_bdd_variables);
//      const int number_of_states = this->GetNumberOfStates();
//      int to_state, sink_state = this->GetSinkState();
//      bool has_sink = true;
//
//      if (sink_state < 0)
//      {
//        has_sink = false;
//        sink_state = 0;
//      }
//
//      std::vector<char> unary_exception = { '1' };
//      char* statuses = new char[number_of_states + 1];
//      std::vector<std::vector<char> > exceptions = { { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' } };
//
//      for (auto& exception : exceptions)
//      {
//        exception.push_back('\0');
//      }
//
//      dfaSetup(number_of_states, number_of_bdd_variables, indices);
//
//      for (int s = 0; s < this->dfa_->ns; s++)
//      {
//        if (s != sink_state || !has_sink)
//        {
//          to_state = getNextState(s, unary_exception);
//          dfaAllocExceptions(exceptions.size());
//          for (auto& exception : exceptions)
//          {
//            dfaStoreException(to_state, &*exception.begin());
//          }
//          dfaStoreState(sink_state);
//        }
//        else
//        {
//          dfaAllocExceptions(0);
//          dfaStoreState(sink_state);
//        }
//
//        if (IsAcceptingState(s))
//        {
//          statuses[s] = '+';
//        }
//        else
//        {
//          statuses[s] = '-';
//        }
//      }
//
//      statuses[number_of_states] = '\0';
//
//      DFA_ptr temp_dfa = dfaBuild(statuses);
//      int_dfa = dfaMinimize(temp_dfa);
//      dfaFree(temp_dfa);
//
//      int_auto = new IntAutomaton(int_dfa, number_of_bdd_variables);
//
//      if (!has_sink)
//      {
//        for (int i = 0; i < int_dfa->ns; i++)
//        {
//          if (int_dfa->f[i] == 0)
//          {
//            int_dfa->f[i] = -1;
//          }
//        }
//      }
//
//      int_auto->setMinus1(add_minus_one);
//      delete[] indices;
//      indices = nullptr;
//      delete[] statuses;
//      statuses = nullptr;
//      DVLOG(VLOG_LEVEL) << int_auto->GetId() << " = [" << this->id_ << "]->toIntAutomaton(" << number_of_bdd_variables
//                        << ", " << add_minus_one << ")";
//
//      return int_auto;
    }

    BinaryIntAutomaton_ptr UnaryAutomaton::ConvertToBinaryIntAutomaton(std::string var_name, ArithmeticFormula_ptr formula,
                                                                bool add_minus_one)
    {
      BinaryIntAutomaton_ptr binary_auto = nullptr;
      SemilinearSet_ptr semilinear_set = GetSemilinearSet();

      binary_auto = BinaryIntAutomaton::MakeAutomaton(semilinear_set, var_name, formula, true);
//  auto test = BinaryIntAutomaton::MakeAutomaton(semilinear_set, var_name, formula, false);
//
//  binary_auto->inspectAuto();
//  test->inspectAuto();

      if (add_minus_one)
      {
        BinaryIntAutomaton_ptr minus_one_auto = nullptr, tmp_auto = nullptr;
        ArithmeticFormula_ptr minus_one_formula = formula->clone();
        minus_one_formula->reset_coefficients();
        minus_one_formula->set_variable_coefficient(var_name, 1);
        minus_one_formula->set_constant(1);
        minus_one_formula->set_type(ArithmeticFormula::Type::EQ);
        minus_one_auto = BinaryIntAutomaton::MakeAutomaton(minus_one_formula, false);
        tmp_auto = binary_auto;
        binary_auto = tmp_auto->Union(minus_one_auto);
        delete tmp_auto;
        tmp_auto = nullptr;
        delete minus_one_auto;
        minus_one_auto = nullptr;
      }

      DVLOG(VLOG_LEVEL) << binary_auto->GetId() << " = [" << this->id_ << "]->toBinaryIntAutomaton(" << var_name << ", "
                        << *binary_auto->get_formula() << ", " << add_minus_one << ")";

      return binary_auto;
    }

    StringAutomaton_ptr UnaryAutomaton::ConvertToStringAutomaton()
    {
      StringAutomaton_ptr result_auto = StringAutomaton::MakePhi(), tmp_1_auto = nullptr, tmp_2_auto = nullptr;

      int sink_state = this->GetSinkState();
      int curr_state { this->dfa_->s };

      std::map<int, bool> is_visited;
      std::queue<int> work_list;
      if (curr_state != sink_state)
      {
        work_list.push(curr_state);
      }

      int value = 0;
      while (not work_list.empty())
      {
        curr_state = work_list.front();
        work_list.pop();

        if (is_visited[curr_state])
        {  // cycle over approximate rest, an algorithm can be found to map between encodings (from semilinear set to string encoding)
          std::string value_str = std::to_string(value);
          std::string regex_str = "[0-9]{" + std::to_string(value_str.length()) + ",}";
          tmp_1_auto = StringAutomaton::MakeRegexAuto(regex_str);
          tmp_2_auto = result_auto;
          result_auto = static_cast<StringAutomaton_ptr>(tmp_2_auto->Concat(tmp_1_auto));
          delete tmp_1_auto;
          delete tmp_2_auto;
          break;
        }

        if (IsAcceptingState(curr_state))
        {
          tmp_1_auto = StringAutomaton::MakeString(std::to_string(value));
          tmp_2_auto = result_auto;
          result_auto = static_cast<StringAutomaton_ptr>(tmp_2_auto->Union(tmp_1_auto));
          delete tmp_1_auto;
          delete tmp_2_auto;
        }
        ++value;
        for (auto next_state : this->GetNextStates(curr_state))
        {
          if (sink_state != next_state)
          {
            work_list.push(next_state);
          }
        }
      }

      DVLOG(VLOG_LEVEL) << result_auto->GetId() << " = [" << this->id_ << "]->toStringAutomaton()";
      return result_auto;
    }

    void UnaryAutomaton::decide_counting_schema(Eigen::SparseMatrix<BigInteger>& count_matrix)
    {
      LOG(FATAL)<<"Not tested";
      counter_.set_type(SymbolicCounter::Type::UNARYINT);
    }

    /** Automaton builder implementation */

    UnaryAutomaton::Builder::Builder()
        : Automaton::Builder(),
          unary_char_ { '1' },
          semilinear_set_ { nullptr }
    {

    }

    UnaryAutomaton::Builder::~Builder()
    {
      delete semilinear_set_;
    }

    UnaryAutomaton::Builder& UnaryAutomaton::Builder::SetUnaryChar(const char c)
    {
      this->unary_char_ = c;
      return *this;
    }

    UnaryAutomaton::Builder& UnaryAutomaton::Builder::SetSemilinearSet(const SemilinearSet_ptr semilinear_set)
    {
      this->semilinear_set_ = semilinear_set;
      return *this;
    }

    UnaryAutomaton_ptr UnaryAutomaton::Builder::Build()
    {
      if (semilinear_set_ and semilinear_set_->is_empty_set())
      {
        DVLOG(VLOG_LEVEL) << *semilinear_set_;
        dfa_ = Libs::MONALib::DFAMakePhi(number_of_bdd_variables_);
      }
      else if (semilinear_set_)
      {
        DVLOG(VLOG_LEVEL) << *semilinear_set_;
        const int cycle_head = semilinear_set_->get_cycle_head();
        const int period = semilinear_set_->get_period();

        number_of_states_ = cycle_head + semilinear_set_->get_period() + 1;
        if (semilinear_set_->has_only_constants())
        {
          number_of_states_ = semilinear_set_->get_constants().back() + 2;
          semilinear_set_->get_periodic_constants().clear();
        }

        int sink_state = number_of_states_ - 1;
        int* indices = Libs::MONALib::GetBddVariableIndices(number_of_bdd_variables_);
        std::string transition(number_of_bdd_variables_, unary_char_);
        std::string statuses(number_of_states_, '-');

        dfaSetup(number_of_states_, number_of_bdd_variables_, indices);
        for (int s = 0; s < number_of_states_ - 2; ++s)
        {
          dfaAllocExceptions(1);
          dfaStoreException(s + 1, const_cast<char*>(transition.data()));
          dfaStoreState(sink_state);
        }

        // Handle last state
        if (semilinear_set_->has_only_constants())
        {
          dfaAllocExceptions(0);
          dfaStoreState(sink_state);
        }
        else
        {
          dfaAllocExceptions(1);
          dfaStoreException(cycle_head, const_cast<char*>(transition.data()));
          dfaStoreState(sink_state);
        }

        dfaAllocExceptions(0);
        dfaStoreState(sink_state);

        for (auto c : semilinear_set_->get_constants())
        {
          CHECK_GE(c, 0);
          statuses[c] = '+';
        }

        for (auto r : semilinear_set_->get_periodic_constants())
        {
          statuses[cycle_head + r] = '+';
        }

        dfa_ = dfaBuild(const_cast<char*>(statuses.data()));
        if (not semilinear_set_->has_only_constants())
        {
          auto tmp_dfa = dfa_;
          dfa_ = dfaMinimize(tmp_dfa);
          dfaFree (tmp_dfa);
        }
      }

      if (dfa_)
      {
        UnaryAutomaton_ptr automaton = new UnaryAutomaton(dfa_, number_of_bdd_variables_);
        dfa_ = nullptr;

        DVLOG(VLOG_LEVEL) << *automaton << " = UnaryAutomaton::Builder::Build()";
        return automaton;
      }

      LOG(FATAL)<< "Automaton cannot be constructed. Make sure minimum required fields are set in order.";
      return nullptr;
    }

  }
/* namespace Theory */
} /* namespace Vlab */
