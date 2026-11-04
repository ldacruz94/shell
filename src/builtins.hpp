#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace shell {

class Shell;

enum class Builtin {
    Echo,
    Exit,
    Type,
};

std::optional<Builtin> builtin_from_name(std::string_view name);
int run_builtin(Shell& sh, Builtin builtin, std::span<const std::string> args);

}