#include <functional>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "util.h"

struct Card {
  int number;
  std::set<int> winning_numbers;
  std::set<int> selected_numbers;
};

std::set<int> parse_numbers(const std::string &s) {
  const auto number_strs = split(s, ' ');
  std::set<int> res;
  std::transform(number_strs.begin(), number_strs.end(),
                 std::inserter(res, res.begin()), [](std::string numstr) {
                   trim(numstr);
                   return std::atoi(numstr.c_str());
                 });
  return res;
}

Card parse_card(const std::string &line) {
  const auto parts = split(line, ':');
  const auto lparts = split(parts[0], ' ');
  auto nparts = split(parts[1], '|');
  std::string game_number_string = lparts[1];
  trim(game_number_string);
  const int game_number = std::atoi(game_number_string.c_str());
  return Card{.number = game_number,
              .winning_numbers = parse_numbers(nparts[0]),
              .selected_numbers = parse_numbers(nparts[1])};
}

int compute_match_count(const Card &card) {
  std::set<int> selected_winners;
  std::set_intersection(
      card.winning_numbers.begin(), card.winning_numbers.end(),
      card.selected_numbers.begin(), card.selected_numbers.end(),
      std::inserter(selected_winners, selected_winners.begin()));
  return selected_winners.size();
}

int compute_points(const Card &card) {
  const int match_count = compute_match_count(card);
  if (match_count == 0) {
    return 0;
  }
  return 1 << (match_count - 1);
}

int part1(const std::vector<std::string> &input) {
  std::vector<Card> cards;
  std::transform(input.begin(), input.end(), std::back_inserter(cards),
                 parse_card);
  std::vector<int> points;
  std::transform(cards.begin(), cards.end(), std::back_inserter(points),
                 compute_points);
  return std::accumulate(points.begin(), points.end(), 0);
}

int part2(const std::vector<std::string> &input) {
  std::vector<Card> cards;
  std::transform(input.begin(), input.end(), std::back_insert_iterator(cards),
                 parse_card);
  for (int i = 0; i < cards.size(); ++i) {
    const auto &current_card = cards[i];
    const int match_count = compute_match_count(current_card);
    const int target_index = current_card.number;
    for (int j = target_index; j < (target_index + match_count); ++j) {
      if (j >= cards.size()) {
        break;
      }
      cards.push_back(cards[j]);
    }
  }
  std::map<int, int> card_counter;
  for (const auto &card : cards) {
    card_counter[card.number] += 1;
  }
  std::cout << "Card counts" << std::endl;
  for (const auto &[card, count] : card_counter) {
    std::cout << "Card " << card << ": " << count << std::endl;
  }
  return cards.size();
}

int main() {
  const auto input = read_input("inputs/day04.txt");
  const int result1 = part1(input);
  const int result2 = part2(input);
  std::cout << "Part 1: " << result1 << std::endl;
  std::cout << "Part 2: " << result2 << std::endl;
  return 0;
}