#include "util/container/fixed_stack.hpp"
#include "gtest/gtest.h"
#include <stdexcept>

TEST(FixedStackTest, TestPush)
{
  using namespace fvp::Utility; 
  FixedStack<int, 5> stk;

  // This should wrap around and overwrite 0 and 1, but our size should be 5 still
  // 5 6 2 3 4
  for(int i = 0; i < 7; ++i)
  {
    stk.push(i);
  }

  EXPECT_EQ(stk[0], 6);
  EXPECT_EQ(stk[1], 5);
  EXPECT_EQ(stk[2], 4);
  EXPECT_EQ(stk[3], 3);
  EXPECT_EQ(stk[4], 2);

  // 0 0 2 3 4
  stk.pop();
  stk.pop();
  EXPECT_EQ(stk[0], 4);
  EXPECT_EQ(stk[1], 3);
  EXPECT_EQ(stk[2], 2);
  
  // 99 0 2 3 4
  stk.push(99);
  EXPECT_EQ(stk[0], 99); 
}

TEST(FixedStackTest, TestSize)
{
  using namespace fvp::Utility; 
  FixedStack<int, 5> stk;

  // This should wrap around and overwrite 0 and 1, but our size should be 5 still
  // 5 6 2 3 4
  for(int i = 0; i < 7; ++i)
  {
    stk.push(i);
  }
  EXPECT_EQ(stk.size(), 5);

  // 0 0 2 3 4
  stk.pop();
  stk.pop();
  EXPECT_EQ(stk.size(), 3);

  // 99 0 2 3 4
  stk.push(99);
  EXPECT_EQ(stk.size(), 4);

  // 0 0 0 0 0, current head is index 2
  stk.pop();
  stk.pop();
  stk.pop();
  stk.pop();
  EXPECT_EQ(stk.size(), 0);
}

TEST(FixedStackTest, TestEmplace)
{
  using namespace fvp::Utility; 
  struct Foo
  {
    int a;
    int b; 
  };

  FixedStack<Foo, 5> stk;

  // This should wrap around and overwrite 0 and 1, but our size should be 5 still
  // 5 6 2 3 4
  for(int i = 0; i < 7; ++i)
  {
    stk.emplace(i, i+1);
  }

  EXPECT_EQ(stk[0].a, 6);
  EXPECT_EQ(stk[0].b, 7);
  EXPECT_EQ(stk[1].a, 5);
  EXPECT_EQ(stk[1].b, 6);
  EXPECT_EQ(stk[2].a, 4);
  EXPECT_EQ(stk[2].b, 5);
  EXPECT_EQ(stk[3].a, 3);
  EXPECT_EQ(stk[3].b, 4);
  EXPECT_EQ(stk[4].a, 2);
  EXPECT_EQ(stk[4].b, 3);
  
  Foo foo{99, 100};

  stk.emplace(foo);
  EXPECT_EQ(stk.top().a, 99);
  EXPECT_EQ(stk.top().b, 100);
}

TEST(FixedStackTest, TestAt)
{
  using namespace fvp::Utility; 
  FixedStack<int, 5> stk;

  // 5 6 2 3 4
  for(int i = 0; i < 7; ++i)
  {
    stk.push(i);
  }

  EXPECT_EQ(stk.at(0), 6);
  EXPECT_EQ(stk.at(1), 5);
  EXPECT_EQ(stk.at(2), 4);
  EXPECT_EQ(stk.at(3), 3);
  EXPECT_EQ(stk.at(4), 2);
  EXPECT_THROW(stk.at(5), std::out_of_range);

  // 0 0 2 3 4
  stk.pop();
  stk.pop();
  EXPECT_EQ(stk.at(0), 4);
  EXPECT_EQ(stk.at(1), 3);
  EXPECT_EQ(stk.at(2), 2);
  EXPECT_THROW(stk.at(3), std::out_of_range);
  EXPECT_THROW(stk.at(4), std::out_of_range);
  EXPECT_THROW(stk.at(5), std::out_of_range);
  
  // 99 0 2 3 4
  stk.push(99);
  EXPECT_EQ(stk.at(0), 99); 
  EXPECT_THROW(stk.at(4), std::out_of_range);
  EXPECT_THROW(stk.at(5), std::out_of_range);

  stk.pop();
  stk.pop();
  stk.pop();
  stk.pop();
  EXPECT_THROW(stk.at(0), std::out_of_range);
}

