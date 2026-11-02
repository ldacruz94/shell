#include "builtins.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <optional>

#include "executor.hpp"
#include "parser.hpp"

namespace shell {

namespace {
    constexpr std::array<std::string_view, 3> kBuiltins = {"echo", "exit", "type"};
}

bool is_builtin(std::string_view name) {
    return std::ranges::find(kBuiltins, name) != kBuiltins.end();
}

int builtin_echo(std::span<const std::string> args) {
    for (std::size_t i = 0; i < args.size(); i++) {
        if (i > 0) {
            std::cout << ' ';
        }
        std::cout << args[i];
    }

    std::cout << '\n';
    return 0;
}

int builtin_exit(std::span<const std::string> args) {
    if (args.size() > 1) {
        std::cerr << "exit: too many arguments\n";
        return 1;
    }

    int code = 0;
    if (!args.empty()) {
        if (const std::optional<int> parsed = parse_int(args[0])) {
            code = *parsed;
        } else {
            std::cerr << "exit: " << args[0] << ": numeric argument required\n";
            code = 2;
        }
    }

    std::exit(code);
}

int builtin_type(std::span<const std::string> args) {
    int status = 0;

    for (const std::string& name : args) {
        if (is_builtin(name)) {
            std::cout << name << " is a shell builtin\n";
        } else if (const auto path = find_in_path(name)) {
            std::cout << name << " is " << path->string() << '\n';
        } else {
            std::cerr << name << ": not found\n";
            status = 1;
        }
    }

    return status;
}

}