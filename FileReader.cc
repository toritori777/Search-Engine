/*
 * Copyright ©2022 Travis McGaha.  All rights reserved.  Permission is
 * hereby granted to the students registered for University of Pennsylvania
 * CIT 595 for use solely during Spring Semester 2022 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include "./HttpUtils.h"
#include "./FileReader.h"

using std::string;

namespace searchserver {

bool FileReader::read_file(string *str) {
  // Read the file into memory, and store the file contents in the
  // output parameter "str."  Be careful to handle binary data
  // correctly; i.e., you probably want to use the two-argument
  // constructor to std::string (the one that includes a length as a
  // second argument).

  // TODO: implement
  std::ifstream file{fname_};
  if (file.fail()){
    return false;
  }
  (*str) = static_cast<std::ostringstream&>(std::ostringstream{} << file.rdbuf()).str();
  if (str == NULL) {
    return false;
  }
  return true;
}  // namespace searchserver
}
