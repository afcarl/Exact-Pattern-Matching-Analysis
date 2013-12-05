/******************************************************************************
 * Tests for pattern matching algorithms
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
typedef bool (*Test) ();  // Pointer to a test function

bool test1() {
  string text = "abbabababa";
  string pattern1 = "aba";
  string pattern2 = "abbc";
  suffixtree::SuffixTree st(text);
  st.Build();

  int match1 = st.Match(pattern1);
  int match2 = st.Match(pattern2);

  return (match1 == 3 && match2 < 0);
}

bool test2() {
  string text = "abcabxabcd";
  string pattern1 = "cd";
  string pattern2 = "cdx";
  suffixtree::SuffixTree st(text);
  st.Build();

  int match1 = st.Match(pattern1);
  int match2 = st.Match(pattern2);

  return (match1 == 8 && match2 < 0);
}

bool test3() {
  string text = "abcdefabxybcdmnabcdex";
  string pattern1 = "cd";
  string pattern2 = "cdx";
  suffixtree::SuffixTree st(text);
  st.Build();

  int match1 = st.Match(pattern1);
  int match2 = st.Match(pattern2);

  return (match1 == 2 && match2 < 0);
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

