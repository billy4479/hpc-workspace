#include "dataset.hpp"
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using std::println;

constexpr auto INPUT_FILE = "./cpp/apps/amsc/dataset.txt";

int main() {
  std::ifstream file(INPUT_FILE);
  if (!file) {
    println("error opening the file");
    return 1;
  }

  std::vector<Dataset> datasets;
  std::string line;

  while (std::getline(file, line)) {
    std::istringstream line_stream(line);

    {
      bool skip = false;
      int next{};
      char tmp{};

      do {
        next = line_stream.peek();
        if (next == EOF || next == '#') {
          skip = true;
          break;
        }

        if (next != ' ') {
          break;
        }
      } while (line_stream.readsome(&tmp, 1) != EOF);

      if (skip) {
        continue;
      }
    }

    try {
      datasets.push_back(read_dataset(line_stream));
    } catch (const std::invalid_argument &e) {
      std::println("invalid dataset: {}", e.what());
    }
  }

  for (const Dataset &d : datasets) {
    d.print();
  }

  return 0;
}
