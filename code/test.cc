/******************************************************************************
 * Test for pattern matching algorithms
 *
 * Copyright 2013, Maruan Al-Shedivat
 ******************************************************************************/
#include <iostream>
#include <string>
#include <vector>
#include "suffixtree/suffix_tree.h"

using std::cout;
using std::endl;
using std::string;

const int kTestNum = 3;   // Number of tests to perform
typedef bool (*Test) ();  // pointer to a test function

bool test1() {
  string text = "abbabababa";
  string pattern1 = "aba";
  string pattern2 = "abbc";
  suffixtree::SuffixTree st(text);
  st.Build();

  int match1 = st.Match(pattern1);
  int match2 = st.Match(pattern2);

  return (match1 >= 0 && match2 < 0);
}

bool test2() {
  // Dummy function body
  // ...
  return true;
}

bool test3() {
  // Dummy function body
  // ...
  return true;
}

int main() {
  Test tests[3] = {test1, test2, test3};
  cout << "Performing tests..." << endl;
  for (int i = 0; i < kTestNum; ++i) {
    if (tests[i]())
      cout << "Test " << i + 1 << " is PASSED" << endl;
    else
      cout << "Test " << i + 1 << " is FAILED" << endl;
  }

  return 0;
}

