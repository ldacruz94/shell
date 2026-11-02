#include "parser.hpp"

#include <charconv>
#include <sstream>

namespace shell {

std::vector<std::string> split_whitespace(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> words;
    std::string word;

    while (stream >> word) {
        words.push_back(word);
    }

    return words;
}

std::optional<int> parse_int(std::string_view text) {
    int value = 0;
    const char* first = text.data();
    const char* last = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(first, last, value);

    if (ec != std::errc{} || ptr != last) {
        return std::nullopt;
    }

    return value;
}

}