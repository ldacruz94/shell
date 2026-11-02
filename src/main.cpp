
#include <iostream>
#include <string>
#include <span>
#include <vector>

#include "builtins.hpp"
#include "executor.hpp"
#include "parser.hpp"

int main() {
    std::string input;
    
    while (true) {
        std::cout << "$ " << std::flush;

        if (!std::getline(std::cin, input)) {
            std::cout << '\n';
            break;
        }

        const std::vector<std::string> words = shell::split_whitespace(input);
        if (words.empty()) {
            continue;
        }

        const std::string& command = words[0];
        const std::span<const std::string> args = std::span(words).subspan(1);
        
        if (command == "exit") {
            shell::builtin_exit(args);
        } else if (command == "echo") {
            shell::builtin_echo(args);
        } else if (command == "type") {
            shell::builtin_type(args);
        } else if (const auto path = shell::resolve_command(command)) {
            shell::run_external(*path, words);
        } else {
            std::cout << command << ": command not found\n";
        }
    }
}