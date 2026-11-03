#include "executor.hpp"
#include "error.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <system_error>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace shell {

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
        throw_errno("fork");
    }

    if (pid == 0) {
        execv(path.c_str(), argv.data());
        const int err = errno;
        std::cerr << words[0] << ": " << std::strerror(err) << '\n';
        _exit(err == ENOENT ? kStatusNotFound : kStatusCannotRun);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        throw_errno("waitpid");
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

}