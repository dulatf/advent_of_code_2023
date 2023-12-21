#include <functional>
#include <iostream>
#include <numeric>

#include "util.h"

struct Set {
  int num_red;
  int num_green;
  int num_blue;
};

struct Game {
  int id;
  std::vector<Set> sets;
};

Set parse_set(std::string s) {
  const std::vector<std::string> parts = split(s, ',');
  Set set{.num_red = 0, .num_green = 0, .num_blue = 0};
  for (std::string part : parts) {
    trim(part);
    const std::vector<std::string> cps = split(part, ' ');
    const int count = atoi(cps[0].c_str());
    const std::string color = cps[1];
    if (color == "blue") {
      set.num_blue += count;
    } else if (color == "red") {
      set.num_red += count;
    } else if (color == "green") {
      set.num_green += count;
    } else {
      throw std::runtime_error(color);
    }
  }
  return set;
}

Game parse_game(std::string line) {
  const std::vector<std::string> lr_parts = split(line, ':');
  const std::vector<std::string> l_parts = split(lr_parts[0], ' ');
  std::string id_part = l_parts[1];
  trim(id_part);
  const int id = std::atoi(id_part.c_str());
  //
  const std::vector<std::string> set_parts = split(lr_parts[1], ';');
  std::vector<Set> sets;
  std::transform(set_parts.begin(), set_parts.end(), std::back_inserter(sets),
                 parse_set);
  return Game{.id = id, .sets = sets};
}

bool is_game_possible(const Game &game, const int max_red, const int max_green,
                      const int max_blue) {
  return std::all_of(game.sets.begin(), game.sets.end(),
                     [max_red, max_green, max_blue](const Set &set) {
                       return set.num_red <= max_red &&
                              set.num_green <= max_green &&
                              set.num_blue <= max_blue;
                     });
}

int part1(const std::vector<Game> &games) {
  std::vector<Game> possible_games;
  std::copy_if(
      games.begin(), games.end(), std::back_inserter(possible_games),
      [](const auto &game) { return is_game_possible(game, 12, 13, 14); });
  std::vector<int> possible_ids;
  std::transform(possible_games.begin(), possible_games.end(),
                 std::back_inserter(possible_ids),
                 [](const auto &game) { return game.id; });
  return std::accumulate(possible_ids.begin(), possible_ids.end(), 0);
}

Set minimum_possible(const Game &game) {
  const int max_red = std::max_element(game.sets.begin(), game.sets.end(),
                                       [](const auto &l, const auto &r) {
                                         return l.num_red < r.num_red;
                                       })
                          ->num_red;
  const int max_green = std::max_element(game.sets.begin(), game.sets.end(),
                                         [](const auto &l, const auto &r) {
                                           return l.num_green < r.num_green;
                                         })
                            ->num_green;
  const int max_blue = std::max_element(game.sets.begin(), game.sets.end(),
                                        [](const auto &l, const auto &r) {
                                          return l.num_blue < r.num_blue;
                                        })
                           ->num_blue;
  return Set{.num_red = max_red, .num_green = max_green, .num_blue = max_blue};
}

int part2(const std::vector<Game> &games) {
  std::vector<int> power;
  std::transform(games.begin(), games.end(), std::back_inserter(power),
                 [](const auto &game) {
                   const auto min_possible = minimum_possible(game);
                   return min_possible.num_red * min_possible.num_green *
                          min_possible.num_blue;
                 });
  return std::accumulate(power.begin(), power.end(), 0);
}

int main() {
  const auto input = read_input("inputs/day02.txt");
  std::vector<Game> games;
  std::transform(input.begin(), input.end(), std::back_insert_iterator(games),
                 parse_game);
  const int result1 = part1(games);
  const int result2 = part2(games);

  std::cout << "Part 1: " << result1 << std::endl;
  std::cout << "Part 2: " << result2 << std::endl;
}