/*
 * SPDX-FileCopyrightText: Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*******************************************************************************
 * Includes
 *******************************************************************************/

#include <pool.hpp>

#include <gtest/gtest.h>

#include <iostream>

/*******************************************************************************
 * Example types
 *******************************************************************************/

typedef char ByteType;
typedef void *PointerType;
typedef char FixedStringType[256];

// A basic struct
struct Point {
    int x, y, z;
};

class Base1 {
  public:
    Base1() : number1(1) { std::cout << __FUNCTION__ << std::endl; }
    virtual ~Base1() { std::cout << __FUNCTION__ << std::endl; }

    virtual void foo1() { std::cout << __FUNCTION__ << std::endl; }
    int getNumber() const { return number1; }

  protected:
    int number1;
};

class Base2 {
  public:
    Base2() : number2(2) { std::cout << __FUNCTION__ << std::endl; }
    virtual ~Base2() { std::cout << __FUNCTION__ << std::endl; }

    virtual void foo2() { std::cout << __FUNCTION__ << std::endl; }
    int getNumber() const { return number2; }

  protected:
    int number2;
};

class Derived : public Base2, public Base1 {
  public:
    Derived() : number3(3) { std::cout << __FUNCTION__ << std::endl; }
    ~Derived() { std::cout << __FUNCTION__ << std::endl; }

    void foo1() override { std::cout << __FUNCTION__ << std::endl; }
    void foo2() override { std::cout << __FUNCTION__ << std::endl; }

    int getNumber1() const { return number1; }
    int getNumber2() const { return number2; }
    int getNumber3() const { return number3; }
    const Point &getPoint() const { return p; }

  private:
    Point p;
    int number3;
};

class NoDefaultConstructor {
  public:
    NoDefaultConstructor(int num) : number(num) {}

    int getNumber() const { return number; }

  private:
    int number;
};

/*******************************************************************************
 * Unit tests
 *******************************************************************************/

TEST(Pool, Base1) {
    Pool<Base1, 2> pool{};
    auto a = pool.makeObject();
    ASSERT_EQ(a->getNumber(), 1);
    pool.destroyObject(a);
}

TEST(Pool, Base2) {
    Pool<Base2, 2> pool{};
    auto a = pool.makeObject();
    ASSERT_EQ(a->getNumber(), 2);
    pool.destroyObject(a);
}

TEST(Pool, Derived) {
    Pool<Derived, 2> pool{};
    auto a = pool.makeObject();
    ASSERT_EQ(a->getNumber1(), 1);
    ASSERT_EQ(a->getNumber2(), 2);
    ASSERT_EQ(a->getNumber3(), 3);
    pool.destroyObject(a);
}

TEST(Pool, NoDefaultConstructor) {
    Pool<NoDefaultConstructor, 2> pool{};
    auto a = pool.makeObject(10);
    ASSERT_EQ(a->getNumber(), 10);
    pool.destroyObject(a);
}

TEST(Pool, Underrun) {
    Pool<Base1, 2> pool{};
    auto a = pool.makeObject();
    ASSERT_NE(a, nullptr);

    auto b = pool.makeObject();
    ASSERT_NE(b, nullptr);

    auto c = pool.makeObject();
    ASSERT_EQ(c, nullptr);

    pool.destroyObject(a);
    pool.destroyObject(b);
    pool.destroyObject(c);
}
