#include "builtins.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <utility>

#include "executor.hpp"
#include "parser.hpp"
#include "shell.hpp"

namespace shell {

namespace {

struct BuiltinEntry {
    std::string_view name;
    Builtin builtin;
};

constexpr std::array kBuiltins = {
    BuiltinEntry{"echo", Builtin::Echo},
    BuiltinEntry{"exit", Builtin::Exit},
    BuiltinEntry{"type", Builtin::Type},
};

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

int builtin_exit(const Shell& sh, std::span<const std::string> args) {
    if (args.size() > 1) {
        std::cerr << "exit: too many arguments\n";
        return 1;
    }

    int code = sh.last_status();
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
        if (builtin_from_name(name)) {
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

std::optional<Builtin> builtin_from_name(std::string_view name) {
    const auto it = std::ranges::find(kBuiltins, name, &BuiltinEntry::name);
    
    if (it == kBuiltins.end()) {
        return std::nullopt;
    }

    return it->builtin;
}

int run_builtin(Shell& sh, Builtin builtin, std::span<const std::string> args) {
    switch (builtin) {
        case Builtin::Echo:
            return builtin_echo(args);
        case Builtin::Exit:
            return builtin_exit(sh, args);
        case Builtin::Type:
            return builtin_type(args);
    }
    std::unreachable();
}

}