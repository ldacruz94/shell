#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace shell {
    std::vector<std::string> split_whitespace(const std::string& line);
    std::optional<int> parse_int(std::string_view text);
}