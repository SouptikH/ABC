/*
 * IntegerAutomatonTest.cpp
 *
 *  Created on: Oct 29, 2015
 *      Author: baki
 *   Copyright: Copyright 2015 The ABC Authors. All rights reserved.
 *              Use of this source code is governed license that can
 *              be found in the COPYING file.
 */

#include "IntegerAutomatonTest.h"

namespace Vlab {
namespace Theory {
namespace Test {

class PublicIntegerAutomaton : public IntegerAutomaton {
 public:
  using IntegerAutomaton::IntegerAutomaton;
  using IntegerAutomaton::type_;
  using IntegerAutomaton::is_count_matrix_cached_;
  using IntegerAutomaton::dfa_;
  using IntegerAutomaton::number_of_bdd_variables_;
  using IntegerAutomaton::count_matrix_;
  using IntegerAutomaton::is_natural_number_;
  using IntegerAutomaton::formula_;

};

using namespace ::testing;
using namespace Vlab::Test::Path;

void IntegerAutomatonTest::SetUp() {
}

void IntegerAutomatonTest::TearDown() {
}

TEST_F(IntegerAutomatonTest, ConstructorWith1Args) {
  PublicIntegerAutomaton b_int_auto (false);
  EXPECT_EQ(Automaton::Type::BINARYINT, b_int_auto.type_);
  EXPECT_EQ(nullptr, b_int_auto.formula_);
  EXPECT_EQ(nullptr, b_int_auto.dfa_);
  EXPECT_EQ(false, b_int_auto.is_natural_number_);
  EXPECT_EQ(0, b_int_auto.number_of_bdd_variables_);
}

TEST_F(IntegerAutomatonTest, ConstructorWith3Args) {
  PublicIntegerAutomaton b_int_auto_0(nullptr, 5, true);
  EXPECT_EQ(Automaton::Type::BINARYINT, b_int_auto_0.type_);
  EXPECT_EQ(nullptr, b_int_auto_0.formula_);
  EXPECT_EQ(nullptr, b_int_auto_0.dfa_);
  EXPECT_EQ(true, b_int_auto_0.is_natural_number_);
  EXPECT_EQ(5, b_int_auto_0.number_of_bdd_variables_);

  auto formula = new ArithmeticFormula();
  formula->set_type(ArithmeticFormula::Type::EQ);
  formula->set_constant(7);
  formula->add_variable("x", 1);
  formula->add_variable("y", 2);
  formula->add_variable("z", 3);
  PublicIntegerAutomaton b_int_auto_1(nullptr, formula, true);
  EXPECT_EQ(Automaton::Type::BINARYINT, b_int_auto_1.type_);
  EXPECT_EQ(formula, b_int_auto_1.formula_);
  EXPECT_EQ(nullptr, b_int_auto_1.dfa_);
  EXPECT_EQ(true, b_int_auto_1.is_natural_number_);
  EXPECT_EQ(3, b_int_auto_1.number_of_bdd_variables_);
}


TEST_F(IntegerAutomatonTest, CopyConstructor) {
  auto formula = new ArithmeticFormula();
  formula->set_type(ArithmeticFormula::Type::EQ);
  formula->set_constant(7);
  formula->add_variable("x", 1);
  formula->add_variable("y", 2);
  formula->add_variable("z", 3);

  PublicIntegerAutomaton b_int_auto_0(nullptr, formula, true);
  PublicIntegerAutomaton b_int_auto_1(b_int_auto_0);
  EXPECT_EQ(Automaton::Type::BINARYINT, b_int_auto_1.type_);
  EXPECT_THAT(b_int_auto_1.formula_->get_variable_coefficient_map(), ElementsAre(Pair("x", 1), Pair("y", 2), Pair("z", 3)));
  EXPECT_EQ(nullptr, b_int_auto_1.dfa_);
  EXPECT_EQ(true, b_int_auto_1.is_natural_number_);
  EXPECT_EQ(3, b_int_auto_1.number_of_bdd_variables_);
}

TEST_F(IntegerAutomatonTest, MakePhi) {
  auto formula = new ArithmeticFormula();
  formula->set_type(ArithmeticFormula::Type::EQ);
  formula->set_constant(7);
  formula->add_variable("x", 1);
  formula->add_variable("y", 2);
  formula->add_variable("z", 3);

  auto result = IntegerAutomaton::MakePhi(formula, false);
  PublicIntegerAutomaton* presult = static_cast<PublicIntegerAutomaton*>(result);

  EXPECT_NE(nullptr, presult->dfa_);
  EXPECT_EQ(1, presult->dfa_->ns);
  EXPECT_EQ(-1, presult->dfa_->f[0]);
  // TODO add an automaton check wrt a expectation
}

//TEST_F(IntegerAutomatonTest, Complement) {
//  std::stringstream ss;
//    std::string expected;
//    TestableIntegerAutomaton testable_binary_automaton;
//    IntegerAutomaton_ptr auto_1 = nullptr;
//    IntegerAutomaton_ptr auto_2 = nullptr;
//    ArithmeticFormula formula_0;
//    formula_0.set_variable_coefficient("x", 1);
//    formula_0.set_constant(-3);
//    formula_0.set_type(ArithmeticFormula::Type::EQ);
//
//    auto_1 = testable_binary_automaton.makeEquality(formula_0.clone());
//    auto_2 = auto_1->complement();
//    auto_2->toDot(ss);
//    expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeNotEquality_01.dot");
//    EXPECT_THAT(expected, ss.str());
//    EXPECT_EQ(ArithmeticFormula::Type::NOTEQ, auto_2->getFormula()->get_type());
//    delete auto_1;
//    delete auto_2;
//
//
//    formula_0.set_constant(3);
//    auto_1 = testable_binary_automaton.makeEquality(formula_0.clone());
//    auto_2 = auto_1->complement();
//
//    ss.str("");
//    auto_2->toDot(ss);
//    expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeNotEquality_02.dot");
//    EXPECT_THAT(expected, ss.str());
//    EXPECT_EQ(ArithmeticFormula::Type::NOTEQ, auto_2->getFormula()->get_type());
//    delete auto_1;
//    delete auto_2;
//
//    ArithmeticFormula formula_1;
//    formula_1.set_variable_coefficient("x", 1);
//    formula_1.set_variable_coefficient("y", 2);
//    formula_1.set_constant(-6);
//    formula_1.set_type(ArithmeticFormula::Type::NOTEQ);
//    auto_1 = TestableIntegerAutomaton::makeNotEquality(formula_1.clone());
//    auto_2 = auto_1->complement();
//
//    ss.str("");
//    auto_2->toDot(ss);
//    expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeEquality_03.dot");
//    EXPECT_THAT(expected, ss.str());
//    EXPECT_EQ(ArithmeticFormula::Type::EQ, auto_2->getFormula()->get_type());
//    delete auto_1;
//    delete auto_2;
//
//    formula_1.set_variable_coefficient("y", -2);
//    auto_1 = TestableIntegerAutomaton::makeNotEquality(formula_1.clone());
//    auto_2 = auto_1->complement();
//
//    ss.str("");
//    auto_2->toDot(ss);
//    expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeEquality_04.dot");
//    EXPECT_THAT(expected, ss.str());
//    EXPECT_EQ(ArithmeticFormula::Type::EQ, auto_2->getFormula()->get_type());
//    delete auto_1;
//    delete auto_2;
//}
//
//TEST_F(IntegerAutomatonTest, Intersect) {
//  std::stringstream ss;
//    std::string expected;
//    TestableIntegerAutomaton testable_binary_automaton;
//    IntegerAutomaton_ptr auto_1 = nullptr;
//    IntegerAutomaton_ptr auto_2 = nullptr;
//    IntegerAutomaton_ptr auto_3 = nullptr;
//    ArithmeticFormula formula_0;
//    formula_0.set_variable_coefficient("x", 1);
//    formula_0.set_constant(-3);
//    formula_0.set_type(ArithmeticFormula::Type::GE);
//
//    ArithmeticFormula formula_1;
//    formula_1.set_variable_coefficient("y", 2);
//    formula_1.set_constant(-6);
//    formula_1.set_type(ArithmeticFormula::Type::NOTEQ);
//
//    ArithmeticFormula formula_2;
//    formula_2.set_variable_coefficient("x", 1);
//    formula_2.set_constant(-3);
//    formula_2.set_type(ArithmeticFormula::Type::LE);
//
//    auto_1 = testable_binary_automaton.makeAutomaton(formula_0.clone());
//    auto_2 = testable_binary_automaton.makeAutomaton(formula_1.clone());
//    EXPECT_DEATH(auto_1->intersect(auto_2), ".*You cannot intersect binary automata with different variable orderings.*");
//    delete auto_2;
//
//    auto_2 = testable_binary_automaton.makeAutomaton(formula_2.clone());
//    auto_3 = auto_1->intersect(auto_2);
//    delete auto_1;
//    delete auto_2;
//
//    formula_0.set_type(ArithmeticFormula::Type::EQ);
//    auto_1 = testable_binary_automaton.makeAutomaton(formula_0.clone());
//
//    EXPECT_EQ(ArithmeticFormula::Type::INTERSECT, auto_3->getFormula()->get_type());
//
//    auto_2 = auto_3->difference(auto_1);
//    EXPECT_TRUE(auto_2->isEmptyLanguage());
//
//    delete auto_1;
//    delete auto_2;
//    delete auto_3;
//}
//
//TEST_F(IntegerAutomatonTest, Union) {
//  std::stringstream ss;
//    std::string expected;
//    TestableIntegerAutomaton testable_binary_automaton;
//    IntegerAutomaton_ptr auto_1 = nullptr;
//    IntegerAutomaton_ptr auto_2 = nullptr;
//    IntegerAutomaton_ptr auto_3 = nullptr;
//    ArithmeticFormula formula_0;
//    formula_0.set_variable_coefficient("x", 1);
//    formula_0.set_constant(-3);
//    formula_0.set_type(ArithmeticFormula::Type::LT);
//
//    ArithmeticFormula formula_1;
//    formula_1.set_variable_coefficient("y", 2);
//    formula_1.set_constant(-6);
//    formula_1.set_type(ArithmeticFormula::Type::NOTEQ);
//
//    ArithmeticFormula formula_2;
//    formula_2.set_variable_coefficient("x", 1);
//    formula_2.set_constant(-3);
//    formula_2.set_type(ArithmeticFormula::Type::GT);
//
//    auto_1 = testable_binary_automaton.makeAutomaton(formula_0.clone());
//    auto_2 = testable_binary_automaton.makeAutomaton(formula_1.clone());
//    EXPECT_DEATH(auto_1->intersect(auto_2), ".*You cannot intersect binary automata with different variable orderings.*");
//    delete auto_2;
//
//    auto_2 = testable_binary_automaton.makeAutomaton(formula_2.clone());
//    auto_3 = auto_1->union_(auto_2);
//
//    delete auto_1;
//    delete auto_2;
//
//    formula_0.set_type(ArithmeticFormula::Type::NOTEQ);
//    auto_1 = testable_binary_automaton.makeAutomaton(formula_0.clone());
//
//    EXPECT_EQ(ArithmeticFormula::Type::UNION, auto_3->getFormula()->get_type());
//    auto_2 = auto_3->difference(auto_1);
//    EXPECT_TRUE(auto_2->isEmptyLanguage());
//
//    delete auto_1;
//    delete auto_2;
//    delete auto_3;
//}
//
//TEST_F(IntegerAutomatonTest, Difference) {
//  std::stringstream ss;
//  std::string expected;
//  std::string actual;
//  TestableIntegerAutomaton testable_binary_automaton;
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  IntegerAutomaton_ptr auto_2 = nullptr;
//  IntegerAutomaton_ptr auto_3 = nullptr;
//  ArithmeticFormula formula_0;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_constant(-3);
//  formula_0.set_type(ArithmeticFormula::Type::LE);
//
//  ArithmeticFormula formula_1;
//  formula_1.set_variable_coefficient("x", 1);
//  formula_1.set_constant(-3);
//  formula_1.set_type(ArithmeticFormula::Type::LT);
//
//  auto_1 = testable_binary_automaton.makeAutomaton(formula_0.clone());
//  auto_2 = testable_binary_automaton.makeAutomaton(formula_1.clone());
//  auto_3 = auto_1->difference(auto_2);
//
//  delete auto_1;
//  delete auto_2;
//
//  formula_0.set_type(ArithmeticFormula::Type::EQ);
//  auto_1 = testable_binary_automaton.makeAutomaton(formula_0.clone());
//
//  auto_2 = auto_3->difference(auto_1);
//  EXPECT_TRUE(auto_2->isEmptyLanguage());
//
//  delete auto_1;
//  delete auto_2;
//  delete auto_3;
//}
//
//TEST_F(IntegerAutomatonTest, GetBinaryAutomatonFor) {
//  std::stringstream ss;
//  std::string expected;
//
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  IntegerAutomaton_ptr auto_2 = nullptr;
//
//  ArithmeticFormula formula_0;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_variable_coefficient("y", -2);
//  formula_0.set_constant(0);
//  formula_0.set_type(ArithmeticFormula::Type::EQ);
//
//  auto_1 = IntegerAutomaton::makeAutomaton(formula_0.clone());
//  auto_2 = auto_1->getBinaryAutomatonFor("x");
//
//  EXPECT_EQ(2, auto_1->getNumberOfVariables());
//  EXPECT_EQ(1, auto_2->getNumberOfVariables());
//
//  auto_2->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "getBinaryAutomatonFor_01.dot");
//  EXPECT_THAT(ss.str(), StrEq(expected));
//
//  delete auto_1;
//  delete auto_2;
//}
//
//TEST_F(IntegerAutomatonTest, MakeEquality) {
//  std::stringstream ss;
//  std::string expected;
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  ArithmeticFormula formula_0;
//  TestableIntegerAutomaton testable_binary_automaton;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_constant(-3);
//  formula_0.set_type(ArithmeticFormula::Type::EQ);
//
//
//  auto_1 = testable_binary_automaton.makeEquality(formula_0.clone());
//
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeEquality_01.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_0.set_constant(3);
//  auto_1 = testable_binary_automaton.makeEquality(formula_0.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeEquality_02.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  ArithmeticFormula formula_1;
//  formula_1.set_variable_coefficient("x", 1);
//  formula_1.set_variable_coefficient("y", 2);
//  formula_1.set_constant(-6);
//  formula_1.set_type(ArithmeticFormula::Type::EQ);
//  auto_1 = testable_binary_automaton.makeEquality(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeEquality_03.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_1.set_variable_coefficient("y", -2);
//  auto_1 = testable_binary_automaton.makeEquality(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeEquality_04.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//}
//
//TEST_F(IntegerAutomatonTest, MakeNotEquality) {
//  std::stringstream ss;
//  std::string expected;
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  ArithmeticFormula formula_0;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_constant(-3);
//  formula_0.set_type(ArithmeticFormula::Type::NOTEQ);
//
//  auto_1 = TestableIntegerAutomaton::makeNotEquality(formula_0.clone());
//
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeNotEquality_01.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_0.set_constant(3);
//  auto_1 = TestableIntegerAutomaton::makeNotEquality(formula_0.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeNotEquality_02.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  ArithmeticFormula formula_1;
//  formula_1.set_variable_coefficient("x", 1);
//  formula_1.set_variable_coefficient("y", 2);
//  formula_1.set_constant(-6);
//  formula_1.set_type(ArithmeticFormula::Type::NOTEQ);
//  auto_1 = TestableIntegerAutomaton::makeNotEquality(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeNotEquality_03.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_1.set_variable_coefficient("y", -2);
//  auto_1 = TestableIntegerAutomaton::makeNotEquality(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeNotEquality_04.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//}
//
//TEST_F(IntegerAutomatonTest, MakeLessThan) {
//  std::stringstream ss;
//  std::string expected;
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  ArithmeticFormula formula_0;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_constant(-3);
//  formula_0.set_type(ArithmeticFormula::Type::LT);
//
//  auto_1 = TestableIntegerAutomaton::makeIntLessThan(formula_0.clone());
//
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThan_01.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_0.set_constant(3);
//  auto_1 = TestableIntegerAutomaton::makeIntLessThan(formula_0.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThan_02.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  ArithmeticFormula formula_1;
//  formula_1.set_variable_coefficient("x", 1);
//  formula_1.set_variable_coefficient("y", 2);
//  formula_1.set_constant(-6);
//  formula_1.set_type(ArithmeticFormula::Type::LT);
//  auto_1 = TestableIntegerAutomaton::makeIntLessThan(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThan_03.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_1.set_variable_coefficient("y", -2);
//  auto_1 = TestableIntegerAutomaton::makeIntLessThan(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThan_04.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//}
//
//TEST_F(IntegerAutomatonTest, MakeLessThanOrEqual) {
//  std::stringstream ss;
//  std::string expected;
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  ArithmeticFormula formula_0;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_constant(-3);
//  formula_0.set_type(ArithmeticFormula::Type::LE);
//
//  auto_1 = TestableIntegerAutomaton::makeLessThanOrEqual(formula_0.clone());
//
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThanOrEqual_01.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_0.set_constant(3);
//  auto_1 = TestableIntegerAutomaton::makeLessThanOrEqual(formula_0.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThanOrEqual_02.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  ArithmeticFormula formula_1;
//  formula_1.set_variable_coefficient("x", 1);
//  formula_1.set_variable_coefficient("y", 2);
//  formula_1.set_constant(-6);
//  formula_1.set_type(ArithmeticFormula::Type::LE);
//  auto_1 = TestableIntegerAutomaton::makeLessThanOrEqual(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThanOrEqual_03.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_1.set_variable_coefficient("y", -2);
//  auto_1 = TestableIntegerAutomaton::makeLessThanOrEqual(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeLessThanOrEqual_04.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//}
//
//TEST_F(IntegerAutomatonTest, MakeGreaterThan) {
//  std::stringstream ss;
//  std::string expected;
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  ArithmeticFormula formula_0;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_constant(-3);
//  formula_0.set_type(ArithmeticFormula::Type::GT);
//
//  auto_1 = TestableIntegerAutomaton::makeGreaterThan(formula_0.clone());
//
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThan_01.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_0.set_constant(3);
//  auto_1 = TestableIntegerAutomaton::makeGreaterThan(formula_0.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThan_02.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  ArithmeticFormula formula_1;
//  formula_1.set_variable_coefficient("x", 1);
//  formula_1.set_variable_coefficient("y", 2);
//  formula_1.set_constant(-6);
//  formula_1.set_type(ArithmeticFormula::Type::GT);
//  auto_1 = TestableIntegerAutomaton::makeGreaterThan(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThan_03.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_1.set_variable_coefficient("y", -2);
//  auto_1 = TestableIntegerAutomaton::makeGreaterThan(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThan_04.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//}
//
//TEST_F(IntegerAutomatonTest, MakeGreaterThanOrEqual) {
//  std::stringstream ss;
//  std::string expected;
//  IntegerAutomaton_ptr auto_1 = nullptr;
//  ArithmeticFormula formula_0;
//  formula_0.set_variable_coefficient("x", 1);
//  formula_0.set_constant(-3);
//  formula_0.set_type(ArithmeticFormula::Type::GE);
//
//  auto_1 = TestableIntegerAutomaton::makeGreaterThanOrEqual(formula_0.clone());
//
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThanOrEqual_01.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_0.set_constant(3);
//  auto_1 = TestableIntegerAutomaton::makeGreaterThanOrEqual(formula_0.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThanOrEqual_02.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  ArithmeticFormula formula_1;
//  formula_1.set_variable_coefficient("x", 1);
//  formula_1.set_variable_coefficient("y", 2);
//  formula_1.set_constant(-6);
//  formula_1.set_type(ArithmeticFormula::Type::GE);
//  auto_1 = TestableIntegerAutomaton::makeGreaterThanOrEqual(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThanOrEqual_03.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//
//  formula_1.set_variable_coefficient("y", -2);
//  auto_1 = TestableIntegerAutomaton::makeGreaterThanOrEqual(formula_1.clone());
//
//  ss.str("");
//  auto_1->toDot(ss);
//  expected = Vlab::Test::FileHelper::getExpectation("theory", "IntegerAutomaton", "makeGreaterThanOrEqual_04.dot");
//  EXPECT_THAT(expected, ss.str());
//  delete auto_1;
//}
//
//TEST_F(IntegerAutomatonTest, MakeAutomaton) {
//  std::vector<int> coeff = {1};
//  std::map<std::string, int> indexes = {{"x", 0}};
//  ArithmeticFormula formula_0(indexes, coeff);
//  formula_0.set_constant(3);
//  formula_0.set_type(ArithmeticFormula::Type::NONE);
//
//  EXPECT_DEATH(TestableIntegerAutomaton::IntegerAutomaton::makeAutomaton(formula_0.clone()), ".*Equation type is not specified, please set type for input formula.*");
//
//  // TODO mocking will allow us to write call expectations
//}



} /* namespace Test */
} /* namespace Theory */
} /* namespace Vlab */
