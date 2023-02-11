#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>

using namespace std;

int isNumber(const string & str)
{
  for (char const & c : str) {
    if (isdigit(c) == 0) {
      return false;
    }
  }

  return true;
}

template<typename Base, typename T>
inline bool instanceof(const T * ptr)
{
  return dynamic_cast<const Base *>(ptr) != nullptr;
}

int input()
{
  string str;
  cin >> str;
  return isNumber(str) ? atoi(str.c_str()) : 0;
}

int pressEnterToContinue()
{
  cout << "Press Enter to continue" << endl;
  cin.ignore();
  cin.get();
}

#endif // UTILS_H
