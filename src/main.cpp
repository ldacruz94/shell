
#include <iostream>
#include <string>
#include <string_view>
#include <iomanip>
#include <span>
#include <sstream>
#include <vector>
#include <charconv>
#include <cstdlib>
#include <optional>
#include <algorithm>
#include <array>
#include <filesystem>
#include <system_error>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

constexpr std::array<std::string_view, 3> kBuiltins = {"echo", "exit", "type"};
const int kCommandNotFound = 126;
const int kCommandCouldNotRun = 125;

bool is_builtin(std::string_view name) {
    return std::ranges::find(kBuiltins, name) != kBuiltins.end();
}

std::optional<fs::path> find_in_path(std::string_view name) {
    const char* path_env = std::getenv("PATH");
    if (path_env == nullptr) {
        return std::nullopt;
    }

    std::string_view dirs = path_env;
    while (true) {
        const std::size_t colon = dirs.find(':');
        const std::string_view dir = dirs.substr(0, colon);

        const fs::path dir_path = dir.empty() ? fs::path(".") : fs::path(dir);
        const fs::path candidate = dir_path / name;

        std::error_code ec;
        if (fs::is_regular_file(candidate, ec) && access(candidate.c_str(), X_OK) == 0) {
            return candidate;
        }

        if (colon == std::string_view::npos) {
            break;
        }
        dirs.remove_prefix(colon + 1);
    }
    return std::nullopt;
}

std::optional<fs::path> resolve_command(std::string_view name) {
    if (name.find('/') != std::string_view::npos) {
        return fs::path(name);
    }
    return find_in_path(name);
}

int run_external(const fs::path& path, const std::vector<std::string>& words) {
    std::vector<char*> argv;

    argv.reserve(words.size() + 1);
    for (const std::string& word : words) {
        argv.push_back(const_cast<char*>(word.c_str()));
    }
    argv.push_back(nullptr);

    std::cout << std::flush;

    const pid_t pid = fork();
    if (pid == -1) {
        std::perror("fork");
        return 1;
    }

    if (pid == 0) {
        execv(path.c_str(), argv.data());
        const int err = errno;
        std::cerr << words[0] << ": " << std::strerror(err) << '\n';
        _exit(err == ENOENT ? kCommandNotFound : kCommandCouldNotRun);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        std::perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        const int sig = WTERMSIG(status);
        std::cerr << strsignal(sig) << '\n';
        return 128 + sig;
    }

    return 1;
}

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
        
        if (command == "exit") {
            if (args.size() > 1) {
                std::cerr << "exit: too many arguments\n";
                continue;
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
        } else if (command == "echo") {
            for (std::size_t i = 0; i < args.size(); i++) {
                if (i > 0) {
                    std::cout << ' ';
                }
                std::cout << args[i];
            }
            std::cout << '\n';
        } else if (command == "type") {
            for (const std::string& name : args) {
                if (is_builtin(name)) {
                    std::cout << name << " is a shell builtin\n";
                } else if (const std::optional<fs::path> path = find_in_path(name)) {
                    std::cout << name << " is " << path->string() << '\n';
                } else {
                    std::cerr << name << ": not found\n";
                }
            }
        } else if (const std::optional<fs::path> path = resolve_command(command)) {
            run_external(*path, words);
        } else {
            std::cout << command << ": command not found\n";
        }
    }
}