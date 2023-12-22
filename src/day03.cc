#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "util.h"

struct Coordinate {
  int row;
  int col;
  bool operator==(const Coordinate &other) const {
    return row == other.row && col == other.col;
  }
};
template <>
struct std::hash<Coordinate> {
  std::size_t operator()(const Coordinate &item) const {
    const std::size_t h1 = std::hash<int>{}(item.row);
    const std::size_t h2 = std::hash<int>{}(item.col);
    return h1 ^ (h2 << 1);
  }
};

struct PositionedNumber {
  int number;
  Coordinate position;
  bool operator==(const PositionedNumber &other) const {
    return number == other.number && position == other.position;
  }
};
template <>
struct std::hash<PositionedNumber> {
  std::size_t operator()(const PositionedNumber &item) const {
    const std::size_t h1 = std::hash<int>{}(item.number);
    const std::size_t h2 = std::hash<Coordinate>{}(item.position);
    return h1 ^ (h2 << 1);
  }
};

std::vector<Coordinate> neighboring_part(const std::vector<std::string> &lines,
                                         int row, int col) {
  const std::vector<std::tuple<int, int>> coords = {
      {row - 1, col - 1}, {row - 1, col},    {row - 1, col + 1},
      {row, col - 1},     {row, col + 1},    {row + 1, col - 1},
      {row + 1, col},     {row + 1, col + 1}};

  std::vector<Coordinate> neighbors;
  for (const auto &[nr, nc] : coords) {
    if (nr < 0 || nc < 0 || (nr >= lines.size() - 1) ||
        (nc >= lines[0].size() - 1)) {
      continue;
    }
    const char c = lines[nr][nc];
    if (!std::isdigit(c) && c != '.') {
      neighbors.push_back(Coordinate{.row = nr, .col = nc});
    }
  }
  return neighbors;
}

std::unordered_map<PositionedNumber, std::vector<Coordinate>> find_numbers(
    const std::vector<std::string> &lines) {
  std::unordered_map<PositionedNumber, std::vector<Coordinate>> numbers;
  std::string current_number;
  std::unordered_set<Coordinate> current_neighbors;
  Coordinate current_coords;
  for (int row = 0; row < lines.size(); ++row) {
    for (int col = 0; col < lines[row].size(); ++col) {
      const char c = lines[row][col];
      if (std::isdigit(c)) {
        if (current_number.empty()) {
          current_coords = Coordinate{.row = row, .col = col};
        }
        current_number += c;
        const auto neighbors = neighboring_part(lines, row, col);
        for (const auto &neighbor : neighbors) {
          current_neighbors.insert(neighbor);
        }
      } else {
        if (current_number.size() > 0) {
          const int num = std::atoi(current_number.c_str());
          std::vector<Coordinate> neighbors;
          neighbors.insert(neighbors.end(), current_neighbors.begin(),
                           current_neighbors.end());
          const PositionedNumber pn{.number = num, .position = current_coords};
          numbers[pn] = neighbors;

          current_number.clear();
          current_neighbors.clear();
        }
      }
    }
  }
  return numbers;
}

int part1(const std::vector<std::string> &lines) {
  const auto numbers = find_numbers(lines);
  std::vector<int> part_numbers;
  std::transform(numbers.begin(), numbers.end(),
                 std::back_inserter(part_numbers), [](const auto &item) {
                   const auto &[key, value] = item;
                   const auto &[number, _] = key;
                   if (value.empty()) {
                     return 0;
                   }
                   return number;
                 });
  return std::accumulate(part_numbers.begin(), part_numbers.end(), 0);
}

int part2(const std::vector<std::string> &lines) {
  const auto numbers = find_numbers(lines);
  std::unordered_map<Coordinate, std::vector<int>> gear_to_number;
  for (const auto &[pos_number, neighbors] : numbers) {
    for (const auto &neighbor : neighbors) {
      if (lines[neighbor.row][neighbor.col] == '*') {
        gear_to_number[neighbor].push_back(pos_number.number);
      }
    }
  }
  std::vector<int> gear_ratios;
  for (const auto &[_, nums] : gear_to_number) {
    if (nums.size() == 2) {
      gear_ratios.push_back(nums[0] * nums[1]);
    }
  }
  return std::accumulate(gear_ratios.begin(), gear_ratios.end(), 0);
}

int main() {
  const auto input = read_input("inputs/day03.txt");
  const int result1 = part1(input);
  const int result2 = part2(input);
  std::cout << "Part 1: " << result1 << std::endl;
  std::cout << "Part 2: " << result2 << std::endl;
  return 0;
}