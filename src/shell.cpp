#include "shell.hpp"

#include <exception>
#include <iostream>
#include <span>

#include "builtins.hpp"
#include "executor.hpp"
#include "parser.hpp"
#include "error.hpp"

namespace shell {

int Shell::run() {
    std::string input;
    
    while (true) {
        std::cout << "$ " << std::flush;

        if (!std::getline(std::cin, input)) {
            std::cout << '\n';
            return last_status_;
        }

        const std::vector<std::string> words = shell::split_whitespace(input);
        if (words.empty()) {
            continue;
        }

        try {
            last_status_ = run_command(words);
        } catch (const std::exception& e) {
            std::cerr << "shell: " << e.what() << '\n';
            last_status_ = 1;
        }
    }
}

int Shell::run_command(const std::vector<std::string>& words) {
    const std::string& command = words[0];
    const std::span<const std::string> args = std::span(words).subspan(1);

    if (const auto builtin = shell::builtin_from_name(command)) {
        return shell::run_builtin(*this, *builtin, args);
    }
    if (const auto path = shell::resolve_command(command)) {
        return shell::run_external(*path, words);
    }

    std::cerr << command << ": command not found\n";
    return kStatusNotFound;
}

}