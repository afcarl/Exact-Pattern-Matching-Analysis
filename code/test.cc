/******************************************************************************
 * Test for pattern matching algorithms
 *
 * Copyright 2013, Maruan Al-Shedivat
 ******************************************************************************/
#include <iostream>
#include <string>
#include <vector>
// to include our algorithms files

using std::cout;
using std::endl;

const int kTestNum = 3;       // Number of tests to perform
typedef bool (*Test) ();  // pointer to a test function

bool test1() {
  // Dummy function body
  cout << "Performed test1!" << endl;
  // ...
  return true;
}

bool test2() {
  // Dummy function body
  cout << "Performed test2!" << endl;
  // ...
  return true;
}

bool test3() {
  // Dummy function body
  cout << "Performed test3!" << endl;
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

