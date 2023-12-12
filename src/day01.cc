#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> read_input(std::string path) {
  std::ifstream infile(path);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(infile, line)) {
    lines.push_back(line);
  }
  return lines;
}

std::vector<int> numbers(std::string line) {
  std::vector<char> num_chars;
  std::copy_if(std::begin(line), std::end(line), std::back_inserter(num_chars),
               [](const auto &c) { return std::isdigit(c); });
  std::vector<int> numbers;
  std::transform(std::begin(num_chars), std::end(num_chars),
                 std::back_inserter(numbers),
                 [](const auto &c) { return static_cast<int>(c - '0'); });
  return numbers;
}

int part1(const std::vector<std::string> &input) {
    int total = 0;
  for (const auto &line : input) {
    const auto nums = numbers(line);
    const int calibration_number = 10 * nums[0] + nums[nums.size() - 1];
    std::cout << line << " -> " << calibration_number << std::endl;
    total += calibration_number;
  }
  return total;
}

int main() {
  const auto input = read_input("inputs/day01.txt");
  const int part1_result = part1(input);
  std::cout << "Part 1: " << part1_result << std::endl;
}