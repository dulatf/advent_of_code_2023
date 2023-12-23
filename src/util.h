#pragma once
#include <algorithm>
#include <fstream>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> tokens;
  std::istringstream token_stream(s);
  std::string token;
  while (std::getline(token_stream, token, delim)) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}

void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) {
            return !std::isspace(c);
          }));
}

void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char c) { return !std::isspace(c); })
              .base(),
          s.end());
}

void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

std::vector<std::string> read_input(std::string path) {
  std::ifstream infile(path);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(infile, line)) {
    lines.push_back(line);
  }
  return lines;
}