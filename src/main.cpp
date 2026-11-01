
#include <iostream>
#include <string>
#include <string_view>
#include <iomanip>
#include <span>
#include <sstream>
#include <vector>

std::vector<std::string> split_whitespace(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> words;
    std::string word;

    while (stream >> word) {
        words.push_back(word);
    }

    return words;
}

int main() {
    std::string input;
    
    while (true) {
        std::cout << "$ " << std::flush;

        if (!std::getline(std::cin, input)) {
            std::cout << '\n';
            break;
        }

        const std::vector<std::string> words = split_whitespace(input);
        if (words.empty()) {
            continue;
        }

        const std::string& command = words[0];
        const std::span<const std::string> args = std::span(words).subspan(1);

        std::cerr << "[debug] commands=" << std::quoted(command) << " args=[";
        for (const std::string& arg : args) {
            std::cerr << ' ' << std::quoted(arg);
        }

        std::cerr << " ]\n";
        
        std::cout << command << ": command not found\n";
    }
}