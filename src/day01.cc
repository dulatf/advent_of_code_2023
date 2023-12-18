#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
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

std::vector<int> numbers_extended(std::string line) {
  std::vector<int> numbers;
  const std::vector<std::tuple<std::string, int>> number_set{
      {"one", 1}, {"two", 2},   {"three", 3}, {"four", 4}, {"five", 5},
      {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}};
  for (int i = 0; i < line.size(); ++i) {
    if (std::isdigit(line[i])) {
      numbers.push_back(static_cast<int>(line[i] - '0'));
    } else {
      for (const auto &[num_str, num_int] : number_set) {
        if (line.substr(i, num_str.size()) == num_str) {
          numbers.push_back(num_int);
          break;
        }
      }
    }
  }
  return numbers;
}

int compute_calibration_number(
    const std::vector<std::string> &input,
    std::function<std::vector<int>(std::string)> get_numbers) {
  int total = 0;
  for (const auto &line : input) {
    const auto nums = get_numbers(line);
    const int calibration_number = 10 * nums[0] + nums[nums.size() - 1];
    // std::cout << line << " -> " << calibration_number << std::endl;
    total += calibration_number;
  }
  return total;
}

int part1(const std::vector<std::string> &input) {
  return compute_calibration_number(
      input, [](std::string line) { return numbers(line); });
}

int part2(const std::vector<std::string> &input) {
  return compute_calibration_number(
      input, [](std::string line) { return numbers_extended(line); });
}

int main() {
  const auto input = read_input("inputs/day01.txt");
  const int part1_result = part1(input);
  std::cout << "Part 1: " << part1_result << std::endl;
  const int part2_result = part2(input);
  std::cout << "Part 2: " << part2_result << std::endl;
}