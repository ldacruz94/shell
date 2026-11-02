#pragma once

#include <span>
#include <string>
#include <string_view>

namespace shell {

bool is_builtin(std::string_view name);

int builtin_echo(std::span<const std::string> args);
int builtin_exit(std::span<const std::string> args);
int builtin_type(std::span<const std::string> args);

}