#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "util.h"

struct Race {
  unsigned long time;
  unsigned long distance;
};

std::vector<Race> parse_races(const std::vector<std::string> &input) {
  std::vector<Race> races;
  std::vector<unsigned long> times;
  std::vector<unsigned long> distances;
  for (const auto &line : input) {
    if (line.find("Time:") != std::string::npos && times.empty()) {
      const auto parts = split(line, ':');
      const auto nums = split(parts[1], ' ');
      std::transform(nums.begin(), nums.end(), std::back_inserter(times),
                     [](const auto &s) { return std::atoi(s.c_str()); });
    } else if (line.find("Distance:") != std::string::npos &&
               distances.empty()) {
      const auto parts = split(line, ':');
      const auto nums = split(parts[1], ' ');
      std::transform(nums.begin(), nums.end(), std::back_inserter(distances),
                     [](const auto &s) { return std::atoi(s.c_str()); });

    } else {
      throw std::runtime_error("Unexpected line" + line);
    }
  }
  if (times.size() != distances.size() || times.empty()) {
    throw std::runtime_error("Invalid table");
  }
  for (int i = 0; i < times.size(); ++i) {
    races.push_back(Race{.time = times[i], .distance = distances[i]});
  }
  return races;
}

// Boat travels as d = v*(t-v) = -v^2+tv
// -> v = 1/2*(t +/- sqrt(t^2-4d))
//
std::tuple<long double, long double>
critical_velocity(const unsigned long time, const unsigned long distance) {
  const unsigned long arg = time * time - 4 * distance;
  std::cout << "arg = " << arg << std::endl;
  const long double root = std::sqrtl(arg);
  std::cout << "root = " << root << std::endl;
  const long double vm = 0.5 * (time - root);
  const long double vp = 0.5 * (time + root);
  return {std::max({vm + 1.e-6, 1.0L}), vp - 1.0e-6};
}

unsigned long count_ways_of_winning(const Race &race) {
  const auto [vm, vp] = critical_velocity(race.time, race.distance);
  const unsigned long uvm = std::ceil(vm);
  const unsigned long uvp = std::floor(vp);
  std::cout << "vm = " << uvm << "\tvp = " << uvp << std::endl;
  return uvp - uvm + 1;
}

unsigned long part1(const std::vector<std::string> &input) {
  const auto races = parse_races(input);
  std::vector<unsigned long> win_counts;
  std::transform(races.begin(), races.end(), std::back_inserter(win_counts),
                 count_ways_of_winning);
  return std::accumulate(win_counts.begin(), win_counts.end(), 1,
                         [](const auto &x, const auto &y) { return x * y; });
}

int main() {
  const auto input = read_input("inputs/day06.txt");
  const int result1 = part1(input);
  // Part 2 is not working due to floating point inaccuracy.
  const auto input2 = read_input("inputs/day06_2.txt");
  const unsigned long result2 = part1(input2);
  std::cout << "Part 1: " << result1 << std::endl;
  std::cout << "Part 2: " << result2 << std::endl;
  return 0;
}
