#include <fstream>
#include <iostream>

#include "experiments.h"
#include "optparser.hpp"
#include "vpparallel.h"
#include "vpserial.cpp"

int main(int argc, char** argv) {
  OptionsParser args(argc, argv);
  args.Parse();
  if (!args.Good()) {
    args.PrintUsage(std::cout);
    return 1;
  }
  args.PrintOptions(std::cout);

  return 0;
}
