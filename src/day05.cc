#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "util.h"

struct Range {
  unsigned long begin;
  unsigned long end;
};

struct MapRange {
  unsigned long destination_start;
  unsigned long source_start;
  unsigned long range_length;
};

struct Map {
  std::vector<MapRange> ranges;
};

unsigned long apply_map(const Map &map, unsigned long source) {
  for (const auto &range : map.ranges) {
    if (source >= range.source_start &&
        source < range.source_start + range.range_length) {
      const unsigned long delta = source - range.source_start;
      return range.destination_start + delta;
    }
  }
  return source;
}

std::vector<Range> map_range(const Map &map, const Range &range) {
  std::vector<MapRange> maps(map.ranges.begin(), map.ranges.end());
  std::sort(maps.begin(), maps.end(), [](const auto &l, const auto &r) {
    return l.source_start < r.source_start;
  });
  std::vector<Range> unmapped_ranges{{range}};
  std::vector<Range> mapped_ranges;
  while (!unmapped_ranges.empty()) {
    Range current_range = unmapped_ranges[unmapped_ranges.size() - 1];
    unmapped_ranges.pop_back();
    bool processed = false;
    for (const auto &map : maps) {
      if (current_range.end <= map.source_start) {
        // Current range is left of all mappers, identity map.
        mapped_ranges.push_back(current_range);
        // No need to check other mappers.
        processed = true;
        break;
      } else if (current_range.begin < map.source_start &&
                 current_range.end > map.source_start &&
                 current_range.end < map.source_start + map.range_length) {
        // Split range and process later.
        unmapped_ranges.push_back(
            Range{.begin = current_range.begin, .end = map.source_start});
        unmapped_ranges.push_back(
            Range{.begin = map.source_start, .end = current_range.end});
        processed = true;
        break;
      } else if (current_range.begin >= map.source_start &&
                 current_range.begin < (map.source_start + map.range_length) &&
                 current_range.end > map.source_start &&
                 current_range.end <= (map.source_start + map.range_length)) {
        // Range is fully contained in current mapper.
        const unsigned long start_delta =
            current_range.begin - map.source_start;
        const unsigned long end_delta = current_range.end - map.source_start;
        mapped_ranges.push_back(
            Range{.begin = map.destination_start + start_delta,
                  .end = map.destination_start + end_delta});
        processed = true;
        break;
      } else if (current_range.begin >= map.source_start &&
                 current_range.begin < (map.source_start + map.range_length) &&
                 current_range.end > map.source_start + map.range_length) {
        // Overlaps to the right, split range and process later.
        unmapped_ranges.push_back(
            Range{.begin = current_range.begin,
                  .end = map.source_start + map.range_length});
        unmapped_ranges.push_back(
            Range{.begin = map.source_start + map.range_length,
                  .end = current_range.end});
        processed = true;
        break;
      } else if (current_range.begin < map.source_start &&
                 current_range.end > (map.source_start + map.range_length)) {
        // Current range fully contains mapper, three way split.
        unmapped_ranges.push_back(
            Range{.begin = current_range.begin, .end = map.source_start});
        unmapped_ranges.push_back(
            Range{.begin = map.source_start,
                  .end = map.source_start + map.range_length});
        unmapped_ranges.push_back(
            Range{.begin = map.source_start + map.range_length,
                  .end = current_range.end});
        processed = true;
        break;
      }
    }
    if (!processed) {
      // If we make it here, the current range is to the right of all mappers,
      // identity map.
      mapped_ranges.push_back(current_range);
    }
  }
  std::sort(mapped_ranges.begin(), mapped_ranges.end(),
            [](const auto &l, const auto &r) { return l.begin < r.begin; });
  return mapped_ranges;
}

struct Almanac {
  std::vector<unsigned long> seeds;
  std::vector<Map> maps;
};

Almanac parse_almanac(const std::vector<std::string> &input) {
  std::vector<unsigned long> seeds;
  std::vector<Map> maps;
  Map current_map{};
  for (const auto &line : input) {
    if (line.empty()) {
      continue;
    }
    if (line.find("seeds:") != std::string::npos) {
      const auto parts = split(line, ':');
      const auto number_parts = split(parts[1], ' ');
      std::transform(number_parts.begin(), number_parts.end(),
                     std::back_inserter(seeds),
                     [](const auto &x) { return std::stoul(x.c_str()); });
    } else if (line.find(":") != std::string::npos) {
      if (!current_map.ranges.empty()) {
        maps.push_back(current_map);
        current_map = Map{};
      }
    } else {
      const auto parts = split(line, ' ');
      if (parts.size() != 3) {
        throw std::runtime_error("Invalid line");
      }
      std::vector<unsigned long> vals;
      std::transform(parts.begin(), parts.end(), std::back_inserter(vals),
                     [](const auto &x) { return std::stoul(x.c_str()); });
      current_map.ranges.push_back(MapRange{.destination_start = vals[0],
                                            .source_start = vals[1],
                                            .range_length = vals[2]});
    }
  }
  if (!current_map.ranges.empty()) {
    maps.push_back(current_map);
  }
  return Almanac{.seeds = seeds, .maps = maps};
}

void print_almanac(const Almanac &almanac) {
  std::cout << "Seeds:";
  for (const auto &x : almanac.seeds) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
  for (const auto &map : almanac.maps) {
    std::cout << "Map:" << std::endl;
    for (const auto &range : map.ranges) {
      std::cout << "Destination start: " << range.destination_start
                << " Source start: " << range.source_start
                << " Length: " << range.range_length << std::endl;
    }
  }
}

unsigned long repeated_apply(const Almanac &almanac, unsigned long source) {
  unsigned long current = source;
  for (const auto &map : almanac.maps) {
    current = apply_map(map, current);
  }
  return current;
}

std::vector<Range> repeated_map_range(const Almanac &almanac,
                                      const std::vector<Range> &input_ranges) {
  std::vector<Range> ranges{input_ranges.begin(), input_ranges.end()};
  int counter = 0;
  for (const auto &map : almanac.maps) {
    std::vector<Range> next;
    for (const auto &range : ranges) {
      const auto res = map_range(map, range);
      next.insert(next.end(), res.begin(), res.end());
    }
    ranges = next;
    ++counter;
  }
  return ranges;
}

unsigned long part1(const std::vector<std::string> &input) {
  const auto almanac = parse_almanac(input);
  print_almanac(almanac);
  std::vector<unsigned long> locations;
  std::transform(almanac.seeds.begin(), almanac.seeds.end(),
                 std::back_inserter(locations),
                 [&almanac](const unsigned long &x) {
                   return repeated_apply(almanac, x);
                 });
  return *std::min_element(locations.begin(), locations.end());
}

unsigned long part2(const std::vector<std::string> &input) {
  const auto almanac = parse_almanac(input);
  std::vector<Range> seed_ranges;
  for (int i = 0; i < almanac.seeds.size() / 2; ++i) {
    seed_ranges.push_back(
        Range{.begin = almanac.seeds[2 * i + 0],
              .end = almanac.seeds[2 * i + 0] + almanac.seeds[2 * i + 1]});
  }
  std::cout << "Seed ranges:\n";
  for (const auto &range : seed_ranges) {
    std::cout << range.begin << ".." << range.end << std::endl;
  }
  const auto mapped = repeated_map_range(almanac, seed_ranges);
  const auto min = std::min_element(
      mapped.begin(), mapped.end(),
      [](const auto &l, const auto &r) { return l.begin < r.begin; });

  return min->begin;
}

int main() {
  const auto input = read_input("inputs/day05.txt");
  const int result1 = part1(input);
  const int result2 = part2(input);
  std::cout << "Part 1: " << result1 << std::endl;
  std::cout << "Part 2: " << result2 << std::endl;
  return 0;
}
