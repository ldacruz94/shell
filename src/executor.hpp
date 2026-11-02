#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace shell {

std::optional<std::filesystem::path> find_in_path(std::string_view name);
std::optional<std::filesystem::path> resolve_command(std::string_view name);
int run_external(const std::filesystem::path& path, const std::vector<std::string>& words);

}